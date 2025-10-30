#include "Network.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <vector>
#include <tuple>
#include <string>

using namespace std;

// === FUNCIONES DE SOPORTE ===

void mostrarTodasLasTablas(Network& red) {
    if (red.routers.empty()) {
        cerr << "La red esta vacia. No hay tablas que mostrar." << endl;
        return;
    }
    for (auto const& [id, router] : red.routers) {
        router->imprimirTabla();
    }
}

// === FUNCIONES COMPARTIDAS (Cambiar ID/Costo) ===

void menuCambiarId(Network& red) {
    if (red.routers.empty()) {
        cerr << "Error: No hay red configurada." << endl;
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
        cerr << "Error: No hay red configurada." << endl;
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


// === LOGICA DE TOPOLOGIA FIJA (OPCION 1) ===

void cargarTopologiaFija(Network& red) {
    const string NOMBRE_ARCHIVO_FIJO = "topologia.txt";
    cout << "\n--- Iniciando Topologia Fija: Cargando el archivo '" << NOMBRE_ARCHIVO_FIJO << "' ---" << endl;

    red.cargarTopologia(NOMBRE_ARCHIVO_FIJO);

    if (!red.routers.empty()) {
        red.actualizarTablas();
        cout << "Topologia Fija cargada. Puede empezar a modificar ID/Costos." << endl;
    } else {
        cerr << "ERROR: No se pudo cargar la topologia fija. Asegurese de que el archivo '" << NOMBRE_ARCHIVO_FIJO << "' existe." << endl;
    }
}

void menuTopologiaFija(Network& red) {
    if (red.routers.empty()) {
        cargarTopologiaFija(red);
    }

    if (red.routers.empty()) return; // Si la carga falló, salir.

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

        // ... (Switch case para opciones 1, 2, 3, 0)
    } while (opcion != 0);
}


// === LOGICA DE TOPOLOGIA MODIFICADA (OPCION 2) ===

void menuCargarTopologia(Network& red) {
    string nombre_archivo;
    cout << "\n--- Cargar Topologia Modificada desde Archivo (Punto B.c) ---" << endl;
    cout << "Ingrese el nombre del archivo de topologia (ej: prueba1.txt): ";
    cin >> nombre_archivo;

    red.cargarTopologia(nombre_archivo);

    if (!red.routers.empty()) {
        red.actualizarTablas();
        cout << "Topologia cargada desde '" << nombre_archivo << "' y tablas calculadas." << endl;
    } else {
        cerr << "ADVERTENCIA: No se pudo cargar la topologia desde el archivo." << endl;
    }
}

void menuAgregarRouter(Network& red) {
    int id;
    cout << "\n--- Agregar Nuevo Router (Punto B.a) ---" << endl;
    cout << "Ingrese el ID (numero) para el nuevo Router: ";
    if (!(cin >> id)) return;

    if (red.getRouter(id)) {
        cerr << "Error: El Router con ID " << id << " ya existe." << endl;
        return;
    }

    red.agregarRouter(id);
    cout << "Router " << id << " agregado exitosamente." << endl;
    red.actualizarTablas();
}

void menuRemoverRouter(Network& red) {
    int id;
    cout << "\n--- Remover Router (Punto B.a) ---" << endl;
    cout << "Ingrese el ID del Router a remover: ";
    if (!(cin >> id)) return;

    red.removerRouter(id);
    red.actualizarTablas();
    cout << "Tablas recalculadas." << endl;
}

void menuAgregarEnlace(Network& red) {
    if (red.routers.empty()) {
        cerr << "Error: No hay routers. Use la opcion 'Cargar Topologia' o 'Agregar Router' primero." << endl;
        return;
    }
    int origen, destino, costo;
    cout << "\n--- Agregar Enlace (Punto B.a) ---" << endl;
    cout << "Origen ID: ";
    if (!(cin >> origen)) return;
    cout << "Destino ID: ";
    if (!(cin >> destino)) return;
    cout << "Costo: ";
    if (!(cin >> costo)) return;

    if (red.getRouter(origen) && red.getRouter(destino) && origen != destino && costo >= 0) {
        red.agregarEnlace(origen, destino, costo);
        red.actualizarTablas();
        cout << "Enlace agregado. Tablas recalculadas." << endl;
    } else {
        cerr << "Error: IDs invalidos (no existen) o costo negativo. No se pudo agregar el enlace." << endl;
    }
}

void menuRemoverEnlace(Network& red) {
    if (red.routers.empty()) {
        cerr << "Error: No hay red configurada." << endl;
        return;
    }
    int origen, destino;
    cout << "\n--- Remover Enlace (Punto B.a) ---" << endl;
    cout << "Origen ID: ";
    if (!(cin >> origen)) return;
    cout << "Destino ID: ";
    if (!(cin >> destino)) return;

    red.removerEnlace(origen, destino);
    red.actualizarTablas();
    cout << "Enlace removido. Tablas recalculadas." << endl;
}


void menuTopologiaModificada(Network& red) {
    int opcion;
    do {
        cout << "\n==============================================" << endl;
        cout << "  MENU - FUNCIONES CON TOPOLOGIA MODIFICADA " << endl;
        cout << "==============================================" << endl;
        cout << "1. Cargar Topologia desde Archivo (B.c)" << endl;
        cout << "2. Agregar Router (B.a)" << endl;
        cout << "3. Remover Router (B.a)" << endl;
        cout << "4. Agregar Enlace (B.a)" << endl;
        cout << "5. Remover Enlace (B.a)" << endl;
        cout << "6. Cambiar Costo de un Enlace" << endl;
        cout << "7. Ver Tablas de Enrutamiento" << endl;
        cout << "0. Regresar al Menu Principal" << endl;
        cout << "Ingrese su opcion: ";

        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            opcion = -1;
        }

        switch (opcion) {
        case 1: menuCargarTopologia(red); break;
        case 2: menuAgregarRouter(red); break;
        case 3: menuRemoverRouter(red); break;
        case 4: menuAgregarEnlace(red); break;
        case 5: menuRemoverEnlace(red); break;
        case 6: menuCambiarCosto(red); break;
        case 7: cout << "\n*** Tablas de Enrutamiento Actuales ***" << endl; mostrarTodasLasTablas(red); break;
        case 0: cout << "Regresando al menu principal." << endl; break;
        default: cerr << "Opcion invalida. Intente de nuevo." << endl; break;
        }
    } while (opcion != 0);
}


// === MENU PRINCIPAL ===

int main() {
    cout << "=== Simulacion de Red de Enrutadores ===" << endl;

    Network red;
    int opcion;

    do {
        cout << "\n==============================================" << endl;
        cout << "  MENU PRINCIPAL " << endl;
        cout << "==============================================" << endl;
        cout << "1. Funciones con Topologia Fija (Carga 'topologia.txt')" << endl;
        cout << "2. Funciones con Topologia Modificada" << endl;
        cout << "3. Funciones con Topologia Aleatoria (Proximamente)" << endl;
        cout << "0. Salir del Programa" << endl;
        cout << "Ingrese su opcion: ";

        if (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            opcion = -1;
        }

        switch (opcion) {
        case 1: menuTopologiaFija(red); break;
        case 2: menuTopologiaModificada(red); break;
        case 3: cerr << "Opcion no implementada aun." << endl; break;
        case 0: cout << "Saliendo del programa. Adios." << endl; break;
        default: cerr << "Opcion invalida. Intente de nuevo." << endl; break;
        }
    } while (opcion != 0);

    return 0;
}
