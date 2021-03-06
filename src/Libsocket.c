#include <hashmap.h>
#include <stdio.h>
#include <stdlib.h>
#include <libwebsockets.h>
#include <bcm2835.h>
#include <curl/curl.h>
#define LED RPI_V2_GPIO_P1_37


typedef void (*Operation)(uint8_t pin);


typedef enum 
{
        Lamp = 21,
        Fan = 20,
        High = 26,
        Iron = 19
} Pins;

typedef struct 
{
        char* item;
        char* operation;
} Command;


static int callback_http(struct lws *wsi,
                         enum lws_callback_reasons reason, void *user,
                         void *in, size_t len)
{
        switch(reason)
        {
                case LWS_CALLBACK_CLIENT_WRITEABLE:
                        printf("client writeable established");
                case LWS_CALLBACK_HTTP: {
                       char *requested_uri = (char*) in;
                       printf("requested uri: %s\n", requested_uri);
                       if(strcmp(requested_uri, "/") == 0)
                       {
                            char *universal_response = "Hi there, how are you today ;)";
                            lws_write(wsi, universal_response, strlen(universal_response), LWS_WRITE_HTTP);
                       }else{
                               char cwd[1024];
                               char *resource_path;
                               if(getcwd(cwd,sizeof(cwd)) != NULL)
                               {
                                    resource_path = malloc(strlen(cwd)+strlen(requested_uri));
                                    
                                   sprintf(resource_path, "%s%s", cwd, requested_uri);

                                    lws_serve_http_file(wsi,resource_path,"text/html",NULL,0); 
                               }
                            }                
                                        }
                default:
                        break;
            }
        return 0;
}

void get_command(char* data, Command *command)
{
        char *token = strtok(data, " ");

        command->item = token;
        token = strtok(NULL," ");
        command->operation = token;
}

Operation get_Function(Command *command)
{
    if(strcmp(command->operation,"Off")==0)
                    return &bcm2835_gpio_set;
    else
        return &bcm2835_gpio_clr;
}

uint8_t get_Pin(Command *command)
{
    if(strcmp(command->item,"Fan")==0)
            return 20;
    else if(strcmp(command->item,"Lamp")==0)
            return 21;
    else if(strcmp(command->item,"High")==0)
            return 19;
    else 
            return 26;
}

void get_updates(unsigned char* status)
{
    uint8_t fan = bcm2835_gpio_lev((uint8_t) Fan);
    uint8_t lamp = bcm2835_gpio_lev((uint8_t) Lamp);
    uint8_t high = bcm2835_gpio_lev((uint8_t) High);
    uint8_t iron = bcm2835_gpio_lev((uint8_t) Iron);
    sprintf(status,"Fan %i,Lamp %i,High %i,Iron %i", fan,lamp,high,iron);
}

static int callback_dumb_increment(struct lws *wsi,
                                   enum lws_callback_reasons reason,
                                   void *user, void *in, size_t len)
{
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: // just log message that someone is connecting
            printf("connection established\n");
            break;
        case LWS_CALLBACK_RECEIVE: { // the funny part
            // create a buffer to hold our response
            // it has to have some pre and post padding. You don't need to care
            // what comes there, lwss will do everything for you. For more info see
            // http://git.warmcat.com/cgi-bin/cgit/lwss/tree/lib/lwss.h#n597
             
             bcm2835_gpio_fsel(21,BCM2835_GPIO_FSEL_OUTP);
             bcm2835_gpio_fsel(20,BCM2835_GPIO_FSEL_OUTP);
             bcm2835_gpio_fsel(26,BCM2835_GPIO_FSEL_OUTP);
             bcm2835_gpio_fsel(19,BCM2835_GPIO_FSEL_OUTP);
             //unsigned char *buf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING);
             
            Command command;
             get_command((unsigned char*)in,&command);
            if(strcmp(command.item,"update")==0)
            {
                    
               unsigned char* updates[100];
               get_updates(updates);
               lws_write(wsi, updates, strlen(updates), LWS_WRITE_TEXT);
            }
            else if(strcmp(command.item,"up")==0 || strcmp(command.item,"down")==0){
                    
            CURL *curl;
            curl = curl_easy_init();
                    char url[50];
                    sprintf("10.0.0.7/%s",command.item);
            curl_easy_setopt(curl,CURLOPT_URL,url);
            curl_easy_perform(curl);
            }
            else
            {
            
            uint8_t pin = get_Pin(&command);

            Operation opr = get_Function(&command); 
             opr(pin);
            }

                         
             int i;
            
            // pointer to `void *in` holds the incomming request
            // we're just going to put it in reverse order and put it in `buf` with
            // correct offset. `len` holds length of the request.
            
            // log what we recieved and what we're going to send as a response.
            // that disco syntax `%.*s` is used to print just a part of our buffer
            // http://stackoverflow.com/questions/5189071/print-part-of-char-array
            
            // send response
            // just notice that we have to tell where exactly our response starts. That's
            // why there's `buf[LWS_SEND_BUFFER_PRE_PADDING]` and how long it is.
            // send response
            // just notice that we have to tell where exactly our response starts. That's
            // why there's `buf[LWS_SEND_BUFFER_PRE_PADDING]` and how long it is.
            // we know that our response has the same length as request because
            // it's the same message in reverse order.
                       
            // release memory back into the wild
            break;
        }
        default:
            break;
    }
    
    return 0;
}



static struct lws_protocols protocols[] = {
    /* first protocol must always be HTTP handler */
    {
        "http-only",   // name
        callback_http, // callback
        0              // per_session_data_size
    },
    {
        "dumb-increment-protocol", // protocol name - very important!
        callback_dumb_increment,   // callback
        0                          // we don't use any per session data
    },
    {
        NULL, NULL, 0   /* End of list */
    }
};

int main(int argc, char ** argv) {

    
        if(!bcm2835_init()) return 1;
    // server url will be http://localhost:9000
    int port =9000;
    struct lws_context *context;
    struct lws_context_creation_info context_info =
    {
        .port = port, .iface = NULL, .protocols = protocols, .extensions = NULL,
        .ssl_cert_filepath = NULL, .ssl_private_key_filepath = NULL, .ssl_ca_filepath = NULL,
        .gid = -1, .uid = -1, .options = 0, NULL, .ka_time = 0, .ka_probes = 0, .ka_interval = 0
    };
    // create lws context representing this server
    context = lws_create_context(&context_info);

    if (context == NULL) {
        fprintf(stderr, "lws init failed\n");
        return -1;
    }
    
    printf("starting server...\n");
    
    // infinite loop, to end this server send SIGTERM. (CTRL+C)
    while (1) {
        lws_service(context, 50);
        // lws_service will process all waiting events with their
        // callback functions and then wait 50 ms.
        // (this is a single threaded webserver and this will keep our server
        // from generating load while there are not requests to process)
    }
    
    lws_context_destroy(context);
return 0;
}
