#include "Network.h"
#include "Router.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <vector>
#include <tuple>
#include <string>
#include <algorithm>

using namespace std;



// Función crucial para limpiar el flujo de entrada en caso de error (e.g., si se ingresa una letra en lugar de un número)
void limpiarEntrada() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void mostrarTodasLasTablas(Network& red) {
    if (red.routers.empty()) {
        cerr << "La red esta vacia. No hay tablas que mostrar." << endl;
        return;
    }
    for (auto const& [id, router] : red.routers) {
        router->imprimirTabla();
    }
}



void menuConsultarRuta(Network& red) {
    if (red.routers.empty()) {
        cerr << "Error: No hay red configurada para consultar rutas." << endl;
        return;
    }

    int origen_id, destino_id;
    cout << "\n--- Consultar Costo y Camino Mas Corto (C y D) ---" << endl;

    cout << "Ingrese el ID del Router Origen: ";
    if (!(cin >> origen_id)) {
        limpiarEntrada();
        cerr << "Error: ID de Origen invalido." << endl;
        return;
    }

    cout << "Ingrese el ID del Router Destino: ";
    if (!(cin >> destino_id)) {
        limpiarEntrada();
        cerr << "Error: ID de Destino invalido." << endl;
        return;
    }

    Router* origen = red.getRouter(origen_id);
    Router* destino = red.getRouter(destino_id);

    if (!origen) {
        cerr << "Error: Router Origen " << origen_id << " no existe." << endl;
        return;
    }
    if (!destino) {
        cerr << "Error: Router Destino " << destino_id << " no existe." << endl;
        return;
    }

    // EJECUTAR DIJKSTRA para el router origen
    origen->dijkstra(&red);

    // Obtener costo
    auto it = origen->tablaEnrutamiento.find(destino_id);
    if (it == origen->tablaEnrutamiento.end()) {
        cerr << "Error: Destino no encontrado en tabla." << endl;
        return;
    }

    int costo_total = it->second.first;

    cout << "\nRESULTADOS DEL ENRUTAMIENTO" << endl;
    cout << "--------------------------------------" << endl;
    cout << "C. Costo Total Mas Bajo: " << (costo_total == numeric_limits<int>::max() ? "INF" : to_string(costo_total)) << endl;

    // Obtener camino
    vector<int> camino = origen->obtenerCaminoMasCorto(destino_id);

    cout << "D. Camino Eficiente (Ruta): ";
    if (camino.empty()) {
        if (costo_total == numeric_limits<int>::max()) {
            cout << "NO ALCANZABLE";
        } else {
            cout << "ERROR EN RUTA";
        }
    } else {
        for (size_t i = 0; i < camino.size(); ++i) {
            cout << camino[i];
            if (i < camino.size() - 1) {
                cout << " -> ";
            }
        }
    }
    cout << endl;
    cout << "--------------------------------------" << endl;
}


void menuCambiarId(Network& red) {
    if (red.routers.empty()) { cerr << "Error: No hay red configurada." << endl; return; }
    int id_actual, nuevo_id;
    cout << "\n--- Cambiar ID de Router ---" << endl;

    cout << "Ingrese el ID actual del Router a modificar: ";
    if (!(cin >> id_actual)) { limpiarEntrada(); return; }

    if (!red.getRouter(id_actual)) { cerr << "Error: Router con ID " << id_actual << " no existe." << endl; return; }

    cout << "Ingrese el nuevo ID para el Router " << id_actual << ": ";
    if (!(cin >> nuevo_id)) { limpiarEntrada(); return; }

    red.cambiarIdRouter(id_actual, nuevo_id);
    red.actualizarTablas();
    cout << "ID cambiado exitosamente. Tablas recalculadas." << endl;
}

void menuCambiarCosto(Network& red) {
    if (red.routers.empty()) { cerr << "Error: No hay red configurada." << endl; return; }
    int c_origen, c_destino, c_nuevo_costo;

    cout << "\n--- Cambiar Costo de Enlace ---" << endl;
    cout << "Ingrese el Router Origen del enlace a modificar: ";
    if (!(cin >> c_origen)) { limpiarEntrada(); return; }

    cout << "Ingrese el Router Destino del enlace a modificar: ";
    if (!(cin >> c_destino)) { limpiarEntrada(); return; }

    cout << "Ingrese el NUEVO COSTO para el enlace directo: ";
    if (!(cin >> c_nuevo_costo)) { limpiarEntrada(); return; }

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
        // Mensaje de error más claro si la carga falla
        cerr << "ERROR FATAL: No se pudo cargar la topologia fija. Asegurese de que el archivo '" << NOMBRE_ARCHIVO_FIJO << "' existe." << endl;
    }
}


/*
void menuTopologiaFija(Network& red) {
    // Si la red esta vacia (ej. al inicio), intenta cargar la topologia fija.
    if (red.routers.empty()) {
        cargarTopologiaFija(red);
    }

    // Si la carga falló (archivo no encontrado), regresa al menu principal inmediatamente.
    if (red.routers.empty()) return;

    int opcion;
    do {
        cout << "\n==============================================" << endl;
        cout << "  MENU - FUNCIONES CON TOPOLOGIA FIJA " << endl;
        cout << "==============================================" << endl;
        cout << "1. Cambiar ID de un Router" << endl;
        cout << "2. Cambiar Costo de un Enlace" << endl;
        cout << "3. Ver Tablas de Enrutamiento" << endl;
        cout << "4. Consultar Costo y Ruta (C y D)" << endl;
        cout << "0. Regresar al Menu Principal" << endl;
        cout << "Ingrese su opcion: ";

        if (!(cin >> opcion)) {
            limpiarEntrada();
            opcion = -1;
        }

        switch (opcion) {
        case 1: menuCambiarId(red); break;
        case 2: menuCambiarCosto(red); break;
        case 3: cout << "\n*** Tablas de Enrutamiento Actuales ***" << endl; mostrarTodasLasTablas(red); break;
        case 4: menuConsultarRuta(red); break;
        case 0: cout << "Regresando al menu principal." << endl; break;
        default: cerr << "Opcion invalida. Intente de nuevo." << endl; break;
        }
    } while (opcion != 0);
}

*/
// === LOGICA DE TOPOLOGIA MODIFICADA (OPCION 2) ===

void menuCargarTopologia(Network& red) {
    string nombre_archivo;
    cout << "\n--- Cargar Topologia Modificada desde Archivo ---" << endl;
    cout << "Ingrese el nombre del archivo de topologia (ej: prueba1.txt): ";
    // La lectura de string se deja asi.
    cin >> nombre_archivo;

    red.cargarTopologia(nombre_archivo);

    if (!red.routers.empty()) {
        red.actualizarTablas();
        cout << "Topologia cargada desde '" << nombre_archivo << "' y tablas calculadas." << endl;
    } else {
        cerr << "ADVERTENCIA: No se pudo cargar la topologia desde el archivo. Verifique el nombre." << endl;
    }
}

void menuAgregarRouter(Network& red) {
    int id;
    cout << "\n--- Agregar Nuevo Router (B.a) ---" << endl;
    cout << "Ingrese el ID (numero) para el nuevo Router: ";
    if (!(cin >> id)) { limpiarEntrada(); return; }

    if (red.getRouter(id)) { cerr << "Error: El Router con ID " << id << " ya existe." << endl; return; }

    red.agregarRouter(id);
    cout << "Router " << id << " agregado exitosamente." << endl;
    red.actualizarTablas();
}

void menuRemoverRouter(Network& red) {
    int id;
    cout << "\n--- Remover Router ---" << endl;
    cout << "Ingrese el ID del Router a remover: ";
    if (!(cin >> id)) { limpiarEntrada(); return; }

    red.removerRouter(id);
    red.actualizarTablas();
    cout << "Tablas recalculadas." << endl;
}

void menuAgregarEnlace(Network& red) {
    if (red.routers.empty()) { cerr << "Error: No hay routers. Use la opcion 'Cargar Topologia' o 'Agregar Router' primero." << endl; return; }
    int origen, destino, costo;
    cout << "\n--- Agregar Enlace ---" << endl;
    cout << "Origen ID: ";
    if (!(cin >> origen)) { limpiarEntrada(); return; }
    cout << "Destino ID: ";
    if (!(cin >> destino)) { limpiarEntrada(); return; }
    cout << "Costo: ";
    if (!(cin >> costo)) { limpiarEntrada(); return; }

    if (red.getRouter(origen) && red.getRouter(destino) && origen != destino && costo >= 0) {
        red.agregarEnlace(origen, destino, costo);
        red.actualizarTablas();
        cout << "Enlace agregado. Tablas recalculadas." << endl;
    } else {
        cerr << "Error: IDs invalidos (no existen) o costo negativo. No se pudo agregar el enlace." << endl;
    }
}

void menuRemoverEnlace(Network& red) {
    if (red.routers.empty()) { cerr << "Error: No hay red configurada." << endl; return; }
    int origen, destino;
    cout << "\n--- Remover Enlace ---" << endl;
    cout << "Origen ID: ";
    if (!(cin >> origen)) { limpiarEntrada(); return; }
    cout << "Destino ID: ";
    if (!(cin >> destino)) { limpiarEntrada(); return; }

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
        cout << "1. Cargar Topologia desde Archivo" << endl;
        cout << "2. Agregar Router " << endl;
        cout << "3. Remover Router " << endl;
        cout << "4. Agregar Enlace " << endl;
        cout << "5. Remover Enlace " << endl;
        cout << "6. Cambiar Costo de un Enlace" << endl;
        cout << "7. Ver Tablas de Enrutamiento" << endl;
        cout << "8. Consultar Costo y Ruta (C y D)" << endl;
        cout << "0. Regresar al Menu Principal" << endl;
        cout << "Ingrese su opcion: ";

        if (!(cin >> opcion)) {
            limpiarEntrada();
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
        case 8: menuConsultarRuta(red); break;
        case 0: cout << "Regresando al menu principal." << endl; break;
        default: cerr << "Opcion invalida. Intente de nuevo." << endl; break;
        }
    } while (opcion != 0);
}

void menuRedAleatoria(Network& red) {
    int num_routers;

    cout << "\n--- Generar Red Aleatoria (E) ---" << endl;

    cout << "Ingrese el número de routers (2-20): ";
    if (!(cin >> num_routers) || num_routers < 2 || num_routers > 20) {
        limpiarEntrada();
        cout << "Numero invalido. Usando valor por defecto: 5" << endl;
        num_routers = 5;
    }


    red.generarRedAleatoria(num_routers);


    red.actualizarTablas();

    cout << "Red aleatoria generada exitosamente con " << num_routers << " routers!" << endl;


    cout << "\n*** Tablas de Enrutamiento de la Red Aleatoria ***" << endl;
    mostrarTodasLasTablas(red);

    cout << "\nAhora puede usar todas las funciones sobre esta red:" << endl;
    menuTopologiaModificada(red);
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
       // cout << "1. Funciones con Topologia Fija (Carga 'topologia.txt')" << endl;
        cout << "2. Funciones con Topologia Modificada" << endl;
        cout << "3. Generar Topologia Aleatoria " << endl;
        cout << "0. Salir del Programa" << endl;
        cout << "Ingrese su opcion: ";

        if (!(cin >> opcion)) {
            limpiarEntrada();
            opcion = -1;
        }

        switch (opcion) {
       // case 1: menuTopologiaFija(red); break;
        case 2: menuTopologiaModificada(red); break;
        case 3: menuRedAleatoria(red); break;
        case 0: cout << "Saliendo del programa. Adios." << endl; break;
        default: cerr << "Opcion invalida. Intente de nuevo." << endl; break;
        }
    } while (opcion != 0);

    return 0;
}
