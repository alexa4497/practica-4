#include "Router.h"
#include "Network.h"
#include <limits>
#include <queue>
#include <string>
#include <algorithm>

using namespace std;

Router::Router(int id) : idRouter(id), previo(nullptr), distancia(numeric_limits<int>::max()), visitado(false) {
}

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

void Router::nuevoVecino(Router* vecino, int costo) {
    for (auto& par : vecinos) {
        if (par.first->idRouter == vecino->idRouter) {
            par.second = costo;
            return;
        }
    }
    vecinos.push_back({vecino, costo});
}

void Router::actualizarCostoVecino(int id_vecino, int nuevo_costo) {
    for (auto& par : vecinos) {
        if (par.first->idRouter == id_vecino) {
            par.second = nuevo_costo;
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

void Router::reinicio() {
    distancia = numeric_limits<int>::max();
    previo = nullptr;
    visitado = false;
}

void Router::dijkstra(Network* network) {
    // 1. REINICIAR todos los routers
    for (auto const& [id, router] : network->routers) {
        router->distancia = numeric_limits<int>::max();
        router->previo = nullptr;
        router->visitado = false;
    }

    // 2. INICIALIZAR router fuente
    this->distancia = 0;

    // 3. Priority_queue para min-heap
    priority_queue<pair<int, Router*>, vector<pair<int, Router*>>, greater<pair<int, Router*>>> pq;
    pq.push({0, this});

    // 4. ALGORITMO DIJKSTRA
    while (!pq.empty()) {
        Router* actual = pq.top().second;
        int dist_actual = pq.top().first;
        pq.pop();

        if (actual->visitado) continue;
        actual->visitado = true;
        actual->distancia = dist_actual;

        // Explorar vecinos
        for (auto& enlace : actual->vecinos) {
            Router* vecino = enlace.first;
            int costo = enlace.second;

            if (!vecino->visitado) {
                int nueva_distancia = dist_actual + costo;

                if (nueva_distancia < vecino->distancia) {
                    vecino->distancia = nueva_distancia;
                    vecino->previo = actual;
                    pq.push({nueva_distancia, vecino});
                }
            }
        }
    }

    // 5. CONSTRUIR TABLA DE ENRUTAMIENTO - MÉTODO CORREGIDO
    this->tablaEnrutamiento.clear();

    for (auto const& [destino_id, destino_router] : network->routers) {
        if (destino_id == this->idRouter) {
            // Para sí mismo
            tablaEnrutamiento[destino_id] = {0, this->idRouter};
        } else if (destino_router->distancia == numeric_limits<int>::max()) {
            // Inalcanzable
            tablaEnrutamiento[destino_id] = {numeric_limits<int>::max(), -1};
        } else {
            // MÉTODO CORRECTO: Encontrar el PRIMER VECINO en el camino
            Router* temp = destino_router;

            // Retroceder hasta el PRIMER router después del origen
            while (temp->previo != nullptr && temp->previo != this) {
                temp = temp->previo;
            }

            // El siguiente salto es el router 'temp'
            tablaEnrutamiento[destino_id] = {destino_router->distancia, temp->idRouter};
        }
    }
}

std::vector<int> Router::obtenerCaminoMasCorto(int destino_id) const {
    vector<int> camino;

    // Verificaciones básicas
    auto it = tablaEnrutamiento.find(destino_id);
    if (it == tablaEnrutamiento.end()) return camino;
    if (it->second.first == numeric_limits<int>::max()) return camino;
    if (destino_id == idRouter) {
        camino.push_back(idRouter);
        return camino;
    }

    // SOLUCIÓN SIMPLE Y EFECTIVA:
    // 1. Siempre empezar con el origen
    camino.push_back(idRouter);

    // 2. Obtener el siguiente salto desde la tabla
    int siguiente_salto = it->second.second;

    // 3. Si el siguiente salto no es el destino, agregarlo
    if (siguiente_salto != -1 && siguiente_salto != destino_id) {
        camino.push_back(siguiente_salto);
    }

    // 4. Siempre terminar con el destino
    if (destino_id != idRouter) {
        camino.push_back(destino_id);
    }

    // Eliminar duplicados consecutivos
    vector<int> camino_limpio;
    for (size_t i = 0; i < camino.size(); i++) {
        if (i == 0 || camino[i] != camino[i-1]) {
            camino_limpio.push_back(camino[i]);
        }
    }

    return camino_limpio;
}
