#include "Network.h"
#include <iostream>
#include <map>
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

    // cout << "\nID del Router cambiado de " << id_actual << " a " << nuevo_id << "." << endl; // Silenciado
}

void Network::actualizarTablas() {
    // El output de recalculando tablas ha sido silenciado.
    for (auto const& [id, router] : routers) {
        router->dijkstra(this);
    }
}

void Network::cargarTopologia(const string& archivo) {
    ifstream file(archivo);
    if (!file.is_open()) {
        cerr << "Error al abrir el archivo de topologia: " << archivo << endl;
        return;
    }

    for (auto const& [id, router] : routers) {
        delete router;
    }
    routers.clear();

    string line;
    bool routers_leidos = false;

    if (getline(file, line) && line.find("ROUTERS:") == 0) {
        string ids_str = line.substr(8);
        stringstream ss(ids_str);
        int id;
        while (ss >> id) {
            agregarRouter(id);
            routers_leidos = true;
        }
    }

    if (!routers_leidos) {
        cerr << "Error: Formato incorrecto o no se encontraron IDs de routers en el archivo." << endl;
        return;
    }

    // cout << "\nCargando enlaces..." << endl; // Silenciado
    while (getline(file, line)) {
        if (line.find("ENLACE:") == 0) {
            stringstream ss(line.substr(7));
            int id1, id2, costo;
            if (ss >> id1 >> id2 >> costo) {
                agregarEnlace(id1, id2, costo);
            }
        }
    }

    file.close();
    // cout << "Carga de topologia desde archivo finalizada." << endl; // Silenciado
}
