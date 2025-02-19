#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <random>
#include <condition_variable>

using namespace std;

// --- Generación de nombres masivos ---
vector<string> generarNombres(int cantidad) {
    vector<string> nombres_base = {
        "Ana", "Carlos", "Beatriz", "Daniel", "Elena", "Fernando", "Gabriela", "Hugo",
        "Isabel", "Javier", "Karla", "Luis", "María", "Nicolás", "Olga", "Pablo",
        "Quintín", "Raquel", "Santiago", "Teresa", "Ulises", "Valeria", "Wilfredo",
        "Ximena", "Yolanda", "Zacarías"
    };

    vector<string> nombres;
    nombres.reserve(cantidad);

    for (int i = 0; i < cantidad; i++) {
        nombres.push_back(nombres_base[i % nombres_base.size()] + to_string(i + 1));
    }

    return nombres;
}

// --- Variables globales ---
vector<string> nombres = generarNombres(1000);  // ?? Ahora son 100,000 nombres
string nombre_buscado = "Pablo50000"; // Un nombre que sí existe en la lista
const int NUM_HILOS = 8;

mutex mtx;
atomic<bool> encontrado(false);
condition_variable cv;
int hilos_terminados = 0;

// --- Función de búsqueda en paralelo ---
void buscar(int id, vector<string> sublista) {
    cout << "Hilo " << id << " buscando en su segmento." << endl;

    for (const auto& nombre : sublista) {
        if (encontrado.load()) {
            cout << "Hilo " << id << " se detiene (otro hilo lo encontró)." << endl;
            return;
        }

        this_thread::sleep_for(chrono::milliseconds(10 + rand() % 30));  // Simula trabajo

        if (nombre == nombre_buscado) {
            lock_guard<mutex> lock(mtx);
            cout << "?? Hilo " << id << " encontró el nombre '" << nombre << "' ??" << endl;
            encontrado.store(true);
            cv.notify_all();
            return;
        }
    }

    cout << "Hilo " << id << " terminó su búsqueda." << endl;

    {
        lock_guard<mutex> lock(mtx);
        hilos_terminados++;
        if (hilos_terminados == NUM_HILOS) {
            cv.notify_all();
        }
    }
}

// --- Supervisor asíncrono ---
void supervisor() {
    for (int i = 0; i < 10; ++i) {  // Más tiempo de monitoreo
        if (encontrado.load()) break;
        cout << "[Supervisor] Seguimos buscando...\n";
        this_thread::sleep_for(chrono::seconds(1));
    }
    cout << "[Supervisor] Finalizando monitoreo." << endl;
}

int main() {
    // Dividir la lista en segmentos
    vector<vector<string>> segmentos(NUM_HILOS);
    for (size_t i = 0; i < nombres.size(); i++) {
        segmentos[i % NUM_HILOS].push_back(nombres[i]);
    }

    // Crear e iniciar los hilos
    vector<thread> hilos;
    for (int i = 0; i < NUM_HILOS; i++) {
        hilos.emplace_back(buscar, i, segmentos[i]);
    }

    // Iniciar supervisor asíncrono
    thread supervisor_thread(supervisor);

    // Esperar a que todos los hilos terminen
    for (auto& hilo : hilos) {
        hilo.join();
    }

    // Esperar a que el supervisor termine
    supervisor_thread.join();

    // Mostrar resultado final
    if (encontrado.load()) {
        cout << "\n? El nombre '" << nombre_buscado << "' fue encontrado." << endl;
    } else {
        cout << "\n? No se encontró el nombre." << endl;
    }

    cout << "?? Búsqueda terminada." << endl;

    // Mantener consola abierta
    cout << "\nPresiona ENTER para salir...";
    cin.ignore();
    cin.get();

    return 0;
}




