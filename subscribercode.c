#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include "MQTTClient.h"

#define ADDRESS     "tcp://127.0.0.1:1883" 
#define CLIENTID    "ExampleClientSub"
#define TOPIC       "test/topic"
#define QOS         1

int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    printf("Message received on topic %s: %.*s\n", topicName, message->payloadlen, (char *)message->payload);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1; 
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client, NULL, NULL, messageArrived, NULL);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    printf("Subscribing to topic %s\n", TOPIC);
    MQTTClient_subscribe(client, TOPIC, QOS);

    printf("Waiting for messages...\n");
    while (1) {
        sleep(1);  
    }

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

    return 0;
}
