#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<libwebsockets.h>

static int callback_http(
                struct lws *wsi,
                enum lws_callback_reasons reason,
                void *user,
                void *in,
                size_t len)
{
        switch(reason)
        {
case LWS_CALLBACK_CLIENT_WRITEABLE:
     printf("connection established\n");

case LWS_CALLBACK_HTTP:{
             char *requested_uri =(char*) in;
             printf("requested URI:%s\n", requested_uri);

             if(strcmp(requested_uri, "/") == 0){
                     void *universal_response = "Hello World!";
                     lws_write(wsi, universal_response,
                                     strlen(universal_response), LWS_WRITE_HTTP);
                     break;
             }else{
                     char cwd[1024];
                     char *resource_path;

                     if(getcwd(cwd,sizeof(cwd)) != NULL){
                             resource_path = malloc(strlen(cwd)+strlen(requested_uri));

                             sprintf(resource_path, "%s%s", cwd, requested_uri);
                             printf("resource_path: %s\n", resource_path);

                             lws_server_http_file(wsi, resource_path, ".html");
                     }
             }

             break;
     }
default:
     printf("unhandled callback\n");
     break;
        }
return 0;
}

static struct lws_protocols protocols[]={
        {
                "http-only",
                callback_http,
                0
        },
        {
                NULL, NULL, 0
        }

};

int main(void)
{
        int port = 8080;
        const char *interface = NULL;
        struct lws_context *context;

        const char *cert_path = NULL;
        const char *key_path = NULL;

        int opts = 0;
        
        struct lws_context_creation_info context_info =
          {
          .port = port, .iface = NULL, .protocols = protocols, .extensions = NULL,
          .ssl_cert_filepath = NULL, .ssl_private_key_filepath = NULL, .ssl_ca_filepath = NULL,
          .gid = -1, .uid = -1, .options = 0, NULL, .ka_time = 0, .ka_probes = 0, .ka_interval = 0
            };
             // create lws context representing this server
                 context = lws_create_context(&context_info);

        if(context == NULL){
                fprintf(stderr, "libwebsocket init failed\n");
                return -1;
        }

        printf("starting server...\n");

        while(1)
        {
                lws_service(context, 50);
        }
        lws_context_destroy(context);
        
        return 0;
}


