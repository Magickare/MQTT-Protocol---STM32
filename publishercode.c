#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "MQTTClient.h"

#define ADDRESS     "tcp://127.0.0.1:1883"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "test/topic"
#define QOS         1
#define TIMEOUT     10000L
#define FILEPATH    "/mnt/c/Users/SIRPI/Desktop/MCW_TEAM_B/data.txt"

pthread_t thread_id;
MQTTClient client;

void* publish_data(void* filepath) {
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        pthread_exit(NULL);
    }

    FILE file = fopen((char)filepath, "r");
    if (file == NULL) {
        perror("Failed to open file");
        MQTTClient_disconnect(client, 10000);
        pthread_exit(NULL);
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        pubmsg.payload = buffer;
        pubmsg.payloadlen = strlen(buffer);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;
        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        printf("Published message: %s\n", buffer);
        rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    }

    fclose(file);
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    pthread_create(&thread_id, NULL, publish_data, (void*)FILEPATH);

    pthread_join(thread_id, NULL);

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

    return 0;
}
