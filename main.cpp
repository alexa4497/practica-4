#include "Network.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <vector>
#include <tuple>

using namespace std;

void mostrarTodasLasTablas(Network& red) {
    for (auto const& [id, router] : red.routers) {
        router->imprimirTabla();
    }
}

void configurarTopologiaFijaAutomatica(Network& red) {
    // Limpiar la red actual
    for (auto const& [id, router] : red.routers) {
        delete router;
    }
    red.routers.clear();

    cout << "\n--- Configurando Topologia Fija de 5 Routers (IDs: 1, 2, 3, 4, 5) ---" << endl;

    // Crear Routers Fijos
    for (int i = 1; i <= 5; ++i) {
        red.agregarRouter(i);
    }

    // Costo por defecto es 1 para todos los enlaces, como se solicitó.
    const int COSTO_POR_DEFECTO = 1;

    // Lista de enlaces de la topología centralizada
    vector<pair<int, int>> enlaces_fijos = {
        {1, 2}, {1, 3}, {1, 4}, {1, 5},
        {2, 3}, {3, 4}, {4, 5}, {5, 2}
    };

    for (const auto& enlace : enlaces_fijos) {
        int origen = enlace.first;
        int destino = enlace.second;

        red.agregarEnlace(origen, destino, COSTO_POR_DEFECTO);
    }

    red.actualizarTablas();
    cout << "Topologia Fija configurada. Costo por defecto para todos los enlaces: " << COSTO_POR_DEFECTO << "." << endl;
}

// ----------------------------------------------------
// Opciones del Menu de Topologia Fija
// ----------------------------------------------------

void menuCambiarId(Network& red) {
    if (red.routers.empty()) {
        cerr << "Error: La red no esta configurada. Configure la topologia fija primero." << endl;
        return;
    }
    int id_actual, nuevo_id;
    cout << "\n--- Cambiar ID de Router ---" << endl;
    cout << "Ingrese el ID actual del Router a modificar: ";
    if (!(cin >> id_actual)) return;

    if (!red.getRouter(id_actual)) {
        cerr << "Error: Router con ID " << id_actual << " no existe." << endl;
        return;
    }

    cout << "Ingrese el nuevo ID para el Router " << id_actual << ": ";
    if (!(cin >> nuevo_id)) return;

    red.cambiarIdRouter(id_actual, nuevo_id);
    red.actualizarTablas();
    cout << "ID cambiado exitosamente. Tablas recalculadas." << endl;
}

void menuCambiarCosto(Network& red) {
    if (red.routers.empty()) {
        cerr << "Error: La red no esta configurada. Configure la topologia fija primero." << endl;
        return;
    }
    int c_origen, c_destino, c_nuevo_costo;

    cout << "\n--- Cambiar Costo de Enlace ---" << endl;
    cout << "Ingrese el Router Origen del enlace a modificar: ";
    if (!(cin >> c_origen)) return;

    cout << "Ingrese el Router Destino del enlace a modificar: ";
    if (!(cin >> c_destino)) return;

    cout << "Ingrese el NUEVO COSTO para el enlace directo: ";
    if (!(cin >> c_nuevo_costo)) return;

    red.cambiarCostoEnlace(c_origen, c_destino, c_nuevo_costo);
    red.actualizarTablas();
    cout << "Costo cambiado exitosamente. Tablas recalculadas." << endl;
}

void menuTopologiaFija(Network& red) {
    // Si la red no ha sido configurada, la configura automáticamente
    if (red.routers.empty()) {
        configurarTopologiaFijaAutomatica(red);
    }

    int opcion;
    do {
        cout << "\n==============================================" << endl;
        cout << "  MENU - FUNCIONES CON TOPOLOGIA FIJA " << endl;
        cout << "==============================================" << endl;
        cout << "1. Cambiar ID de un Router" << endl;
        cout << "2. Cambiar Costo de un Enlace" << endl;
        cout << "3. Ver Tablas de Enrutamiento" << endl;
        cout << "0. Regresar al Menu Principal" << endl;
        cout << "Ingrese su opcion: ";

        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            opcion = -1;
        }

        switch (opcion) {
        case 1:
            menuCambiarId(red);
            break;
        case 2:
            menuCambiarCosto(red);
            break;
        case 3:
            cout << "\n*** Tablas de Enrutamiento Actuales ***" << endl;
            mostrarTodasLasTablas(red);
            break;
        case 0:
            cout << "Regresando al menu principal." << endl;
            break;
        default:
            cerr << "Opcion invalida. Intente de nuevo." << endl;
            break;
        }
    } while (opcion != 0);
}

// ----------------------------------------------------
// Menu Principal
// ----------------------------------------------------

int main() {
    cout << "=== Simulacion de Red de Enrutadores ===" << endl;

    Network red;
    int opcion;

    do {
        cout << "\n==============================================" << endl;
        cout << "  MENU PRINCIPAL " << endl;
        cout << "==============================================" << endl;
        cout << "1. Funciones con Topologia Fija" << endl;
        cout << "2. Funciones con Topologia Modificada (Proximamente)" << endl;
        cout << "3. Funciones con Topologia Aleatoria (Proximamente)" << endl;
        cout << "0. Salir del Programa" << endl;
        cout << "Ingrese su opcion: ";

        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            opcion = -1;
        }

        switch (opcion) {
        case 1:
            menuTopologiaFija(red);
            break;
        case 2:
            cerr << "Opcion no implementada aun. Seleccione la opcion 1." << endl;
            break;
        case 3:
            cerr << "Opcion no implementada aun. Seleccione la opcion 1." << endl;
            break;
        case 0:
            cout << "Saliendo del programa. Adios." << endl;
            break;
        default:
            cerr << "Opcion invalida. Intente de nuevo." << endl;
            break;
        }
    } while (opcion != 0);

    return 0;
}
