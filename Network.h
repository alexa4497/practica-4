#ifndef NETWORK_H
#define NETWORK_H

#include "Router.h"
#include <map>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class Network {
public:
    map<int, Router*> routers;

    ~Network();

    void agregarRouter(int id);
    void agregarEnlace(int id1, int id2, int costo);

    void actualizarTablas();

    void cambiarCostoEnlace(int id1, int id2, int nuevo_costo);

    Router* getRouter(int id);

    void cargarTopologia(const string& archivo);
    void cambiarIdRouter(int id_actual, int nuevo_id);

    void removerRouter(int id);   // <- NUEVA DECLARACIÓN
    void removerEnlace(int id1, int id2);

};

#endif // NETWORK_H
