#ifndef ROUTER_H
#define ROUTER_H

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>

using namespace std;

class Network;

class Router {
public:
    int idRouter;

    vector<pair<Router*, int>> vecinos;

    Router* previo;
    int distancia;
    bool visitado;

    map<int, pair<int, int>> tablaEnrutamiento;

    Router(int id);

    void nuevoVecino(Router* vecino, int costo);
    void actualizarCostoVecino(int id_vecino, int nuevo_costo);

    void reinicio();
    void dijkstra(Network* network);

    void imprimirTabla();

    int getDistancia() const { return distancia; }
    int getId() const { return idRouter; }

    void confDistancia(int dist) { distancia = dist; }
    void removerVecino(int id_vecino);
    std::vector<int> obtenerCaminoMasCorto(int destino_id) const;
};

#endif // ROUTER_H
