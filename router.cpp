#include "Router.h"
#include "Network.h"
#include <limits>
#include <queue>
#include <string>

using namespace std;

using CostoRouter = pair<int, Router*>;


Router::Router(int id) : idRouter(id), previo(nullptr), distancia(numeric_limits<int>::max()), visitado(false) {
}

// Dentro de la implementacion de Router::removerVecino
void Router::removerVecino(int id_vecino) {
    auto it = vecinos.begin();
    while (it != vecinos.end()) {
        if (it->first->idRouter == id_vecino) {
            it = vecinos.erase(it);
            return;
        }
        ++it;
    }
}

// ... (Resto del codigo, incluyendo nuevoVecino y actualizarCostoVecino silenciados)

void Router::nuevoVecino(Router* vecino, int costo) {
    for (auto& par : vecinos) {
        if (par.first->idRouter == vecino->idRouter) {
            par.second = costo;
            // cout << "Costo del enlace " << idRouter << "->" << vecino->idRouter << " actualizado a " << costo << "." << endl; // Eliminado
            return;
        }
    }
    vecinos.push_back({vecino, costo});
    // cout << "Enlace " << idRouter << "->" << vecino->idRouter << " creado con costo " << costo << "." << endl; // Eliminado
}

void Router::actualizarCostoVecino(int id_vecino, int nuevo_costo) {
    for (auto& par : vecinos) {
        if (par.first->idRouter == id_vecino) {
            par.second = nuevo_costo;
            // cout << "\nRouter " << idRouter << ": Costo a vecino " << id_vecino << " cambiado a " << nuevo_costo << "." << endl; // Eliminado
            return;
        }
    }
    cerr << "Router " << idRouter << " no esta conectado directamente al Router " << id_vecino << ". El costo no fue actualizado." << endl;
}

void Router::imprimirTabla() {
    cout << "Tabla de Enrutamiento del Router " << idRouter << " (Fuente):" << endl;
    cout << "----------------------------------------------------" << endl;
    cout << "| Destino | Costo Total | Siguiente Salto |" << endl;
    cout << "----------------------------------------------------" << endl;

    for (const auto& entrada : tablaEnrutamiento) {
        int destino_id = entrada.first;
        int costo = entrada.second.first;
        int siguiente_salto = entrada.second.second;

        string costo_str = (costo == numeric_limits<int>::max()) ? "INF" : to_string(costo);
        string salto_str = (siguiente_salto == -1) ? "-" : to_string(siguiente_salto);

        printf("| %7d | %11s | %15s |\n", destino_id, costo_str.c_str(), salto_str.c_str());
    }
    cout << "----------------------------------------------------" << endl;
}
// ... (resto de Router.cpp: reinicio, dijkstra)

void Router::reinicio() {
    distancia = numeric_limits<int>::max();
    previo = nullptr;
    visitado = false;
}

void Router::dijkstra(Network* network) {
    for (auto const& [id, router] : network->routers) {
        router->reinicio();
    }

    this->distancia = 0;

    priority_queue<CostoRouter> pq;
    pq.push({-this->distancia, this});

    while (!pq.empty()) {
        Router* u = pq.top().second;
        pq.pop();

        if (u->visitado) continue;
        u->visitado = true;

        for (const auto& enlace : u->vecinos) {
            Router* v = enlace.first;
            int peso_u_v = enlace.second;

            int nueva_distancia = u->distancia + peso_u_v;

            if (nueva_distancia < v->distancia) {
                v->distancia = nueva_distancia;
                v->previo = u;
                pq.push({-v->distancia, v});
            }
        }
    }

    this->tablaEnrutamiento.clear();
    for (auto const& [destino_id, destino_router] : network->routers) {
        if (destino_router->idRouter == this->idRouter) {
            this->tablaEnrutamiento[destino_id] = {0, destino_id};
            continue;
        }

        int costo_total = destino_router->distancia;
        int siguiente_salto_id = -1;

        if (costo_total != numeric_limits<int>::max()) {
            Router* actual = destino_router;
            Router* previo_actual = actual->previo;

            while (previo_actual && previo_actual->idRouter != this->idRouter) {
                actual = previo_actual;
                previo_actual = actual->previo;
            }
            siguiente_salto_id = actual->idRouter;
        }

        this->tablaEnrutamiento[destino_id] = {costo_total, siguiente_salto_id};
    }
}


