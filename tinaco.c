#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

// Definición de variables globales
int volumen = 0;
int caudal = 10;
int caudalvaci = 5;
int umbral = 10;
int peligro = 12;
unsigned int tiempoej = 0;
unsigned int tiempo = 0;

pthread_mutex_t mutex;

void* sensor_control(void* arg) {
    while (tiempo < tiempoej) {
        if (volumen >= umbral) {
            printf("Sensor de control: limite alcanzado. Abriendo valvula de evacuacion.\n");
            volumen -= caudalvaci;
            sleep(1);
        }
        if(volumen>caudalvaci){
            volumen -= caudalvaci;
            sleep(1);
        }
        
        sleep(1);
        if (tiempo >= tiempoej) break; // salir del bucle si se alcanzó el tiempo límite
    }
    pthread_exit(NULL);
}

void* alarma_peligro(void* arg) {
    while (tiempo < tiempoej) {
        if (volumen > peligro) {
            printf("Alarma de peligro: volumen peligroso alcanzado.\n");
            volumen -= caudal;
        }
        sleep(1);
        if (tiempo >= tiempoej) break; // salir del bucle si se alcanzó el tiempo límite
    }
    pthread_exit(NULL);
}

void* entrada_agua(void* arg) {
    while (tiempo < tiempoej) {
        if (volumen <= umbral) {
            volumen += caudal;
            if (volumen > peligro) {
                printf("Entrada de agua: volumen peligroso alcanzado. Reduciendo caudal.\n");
                volumen -= caudalvaci;
                while (volumen >= peligro) {
                    sleep(1);
                }
            }
        }
        sleep(1);

        // Detener la entrada de agua cuando se alcance el nivel deseado
        while (volumen > umbral && volumen >= peligro) {
            sleep(1);
            if (tiempo >= tiempoej) break; // salir del bucle si se alcanzó el tiempo límite
        }
        if (tiempo >= tiempoej) break; // salir del bucle principal si se alcanzó el tiempo límite
    }
    pthread_exit(NULL);
}

// Función que se ejecuta en un hilo
void* funcion_principal(void* arg) {
    // Bucle principal
    while (tiempo < tiempoej) {
        printf("Tiempo: %d, Volumen: %d\n", tiempo, volumen);
        tiempo++;
        sleep(1);
    }
    pthread_exit(NULL);
}

// Función principal
int main() {
    printf("ingrese el tiempo de ejecucion\n");
    scanf("%d",&tiempoej);
    // Inicialización del mutex
    pthread_mutex_init(&mutex, NULL);

    // Creación de los threads
    pthread_t controlador, alarma, entrada, principal;
    pthread_create(&controlador, NULL, sensor_control, NULL);
    pthread_create(&alarma, NULL, alarma_peligro, NULL);
    pthread_create(&entrada, NULL, entrada_agua, NULL);
    pthread_create(&principal, NULL, funcion_principal, NULL);

    // Espera a que los threads finalicen
    pthread_join(controlador, NULL);
    pthread_join(alarma, NULL);
    pthread_join(entrada, NULL);
    pthread_join(principal, NULL);

    // Destruccion del mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}