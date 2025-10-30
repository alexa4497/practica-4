#include "Network.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
using namespace std;

Network::~Network() {
    for (auto const& [id, router] : routers) {
        delete router;
    }
    routers.clear();
}

void Network::agregarRouter(int id) {
    if (routers.find(id) == routers.end()) {
        routers[id] = new Router(id);
        // cout << "Router " << id << " creado." << endl; // Silenciado
    }
}

Router* Network::getRouter(int id) {
    if (routers.count(id)) {
        return routers.at(id);
    }
    return nullptr;
}

void Network::agregarEnlace(int id1, int id2, int costo) {
    Router* r1 = getRouter(id1);
    Router* r2 = getRouter(id2);
    if (r1 && r2) {
        r1->nuevoVecino(r2, costo);
        r2->nuevoVecino(r1, costo);
    } else {
        cerr << "Error: Router(s) no encontrado(s) al agregar enlace." << endl;
    }
}

void Network::cambiarCostoEnlace(int id1, int id2, int nuevo_costo) {
    Router* r1 = getRouter(id1);
    Router* r2 = getRouter(id2);
    if (r1 && r2) {
        r1->actualizarCostoVecino(id2, nuevo_costo);
        r2->actualizarCostoVecino(id1, nuevo_costo);
    } else {
        cerr << "Error: Router(s) no encontrado(s) al cambiar costo." << endl;
    }
}

void Network::cambiarIdRouter(int id_actual, int nuevo_id) {
    if (routers.count(nuevo_id)) {
        cerr << "Error: El nuevo ID (" << nuevo_id << ") ya esta en uso." << endl;
        return;
    }

    if (!routers.count(id_actual)) {
        cerr << "Error: El Router con ID " << id_actual << " no existe." << endl;
        return;
    }

    Router* r = routers.at(id_actual);

    r->idRouter = nuevo_id;

    routers[nuevo_id] = r;
    routers.erase(id_actual);


}

void Network::actualizarTablas() {
    // El output de recalculando tablas ha sido silenciado.
    for (auto const& [id, router] : routers) {
        router->dijkstra(this);
    }
}




void Network::removerEnlace(int id1, int id2) {
    Router* r1 = getRouter(id1);
    Router* r2 = getRouter(id2);
    if (r1 && r2) {
        r1->removerVecino(id2);
        r2->removerVecino(id1);
        cout << "Enlace entre " << id1 << " y " << id2 << " removido." << endl;
    } else {
        cerr << "Error: Router(s) no encontrado(s) al intentar remover enlace." << endl;
    }
}

// Implementación para remover un router (debe limpiar todos sus enlaces antes)
void Network::removerRouter(int id) {
    if (!routers.count(id)) {
        cerr << "Error: El Router con ID " << id << " no existe." << endl;
        return;
    }

    Router* router_a_remover = routers.at(id);

    // 1. Eliminar las referencias de este router en sus vecinos (romper todos los enlaces)
    for (const auto& enlace : router_a_remover->vecinos) {
        Router* vecino = enlace.first;
        if (vecino) {
            vecino->removerVecino(id);
        }
    }

    // 2. Eliminar de la lista de routers y liberar memoria
    delete router_a_remover;
    routers.erase(id);

    cout << "Router " << id << " y todos sus enlaces removidos." << endl;
}


void Network::cargarTopologia(const string& archivo) {
    ifstream file(archivo);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo de topologia: " << archivo << endl;
        return;
    }

    // Limpiar la red existente antes de cargar la nueva
    for (auto const& [id, router] : routers) {
        delete router;
    }
    routers.clear();

    string line;
    cout << "Cargando topologia desde archivo..." << endl;

    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        stringstream ss(line);
        string tipo;
        ss >> tipo;

        if (tipo == "ROUTERS:") {
            int id;
            while (ss >> id) {
                agregarRouter(id);
            }
        } else if (tipo == "ENLACE:") {
            int id1, id2, costo;
            if (ss >> id1 >> id2 >> costo) {
                // Si el router no fue listado en la seccion ROUTERS, lo agrega al encontrar un enlace
                if (!routers.count(id1)) agregarRouter(id1);
                if (!routers.count(id2)) agregarRouter(id2);
                agregarEnlace(id1, id2, costo);
            }
        }
    }

    file.close();
    if (routers.empty()) {
        cerr << "ADVERTENCIA: Archivo leido pero no se creo ningun router." << endl;
    }
}
void Network::generarRedAleatoria(int num_routers) {
    // Limpiar red existente
    for (auto const& [id, router] : routers) {
        delete router;
    }
    routers.clear();


    for (int i = 1; i <= num_routers; ++i) {
        agregarRouter(i);
    }

    // 2. Distribución para costos (1-10)
    std::uniform_int_distribution<int> dist_costo(1, 10);
    std::uniform_real_distribution<double> dist_prob(0.0, 1.0);

    // 3. Crear enlaces aleatorios - aproximadamente 40% de densidad
    for (int i = 1; i <= num_routers; ++i) {
        for (int j = i + 1; j <= num_routers; ++j) {
            // 40% de probabilidad de crear enlace
            if (dist_prob(gen) < 0.4) {
                int costo = dist_costo(gen);
                agregarEnlace(i, j, costo);
            }
        }
    }

    // 4. Garantizar que la red esté conectada
    for (int i = 2; i <= num_routers; ++i) {
        bool tiene_enlace = false;
        Router* r = getRouter(i);
        if (r && !r->vecinos.empty()) {
            tiene_enlace = true;
        }

        if (!tiene_enlace) {

            std::uniform_int_distribution<int> dist_router(1, i-1);
            int router_destino = dist_router(gen);
            int costo = dist_costo(gen);
            agregarEnlace(i, router_destino, costo);
        }
    }

    cout << "Red aleatoria generada con " << num_routers << " routers." << endl;
}
