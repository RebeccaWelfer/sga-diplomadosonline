// ========================================================
// SISTEMA DE GESTIÓN ACADÉMICA (SGA-DO)
// Entregable #6: Prototipo Funcional en C++
// Alumna: Rebecca Alexandra Welffer Gutierrez
// ========================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stack>
#include <queue>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace std;

// --- FUNCIÓN AUXILIAR PARA LIMPIAR ESPACIOS ---
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

// --- 1. JERARQUÍA DE PROGRAMAS ACADÉMICOS (Polimorfismo) ---
class ProgramaAcademico {
public:
    string nombre;
    ProgramaAcademico(string nombre) : nombre(nombre) {}
    virtual ~ProgramaAcademico() {}

    virtual bool evaluarAprobacion(const vector<double>& notas) = 0;

    double calcularPromedio(const vector<double>& notas) {
        if (notas.empty()) return 0.0;
        double suma = 0.0;
        for (double n : notas) suma += n;
        return suma / notas.size();
    }
};

class Curso : public ProgramaAcademico {
public:
    Curso() : ProgramaAcademico("Curso") {}
    bool evaluarAprobacion(const vector<double>& notas) override {
        return calcularPromedio(notas) >= 10.0;
    }
};

class Diplomado : public ProgramaAcademico {
public:
    Diplomado() : ProgramaAcademico("Diplomado") {}
    bool evaluarAprobacion(const vector<double>& notas) override {
        return calcularPromedio(notas) >= 14.0;
    }
};

class Bootcamp : public ProgramaAcademico {
public:
    Bootcamp() : ProgramaAcademico("Bootcamp") {}
    bool evaluarAprobacion(const vector<double>& notas) override {
        for (double n : notas) {
            if (n < 14.0) return false;
        }
        return true;
    }
};

ProgramaAcademico* instanciarPrograma(string nombre) {
    string p = toLower(trim(nombre));
    if (p == "curso") return new Curso();
    if (p == "diplomado") return new Diplomado();
    if (p == "bootcamp") return new Bootcamp();
    return nullptr;
}

// --- 2. JERARQUÍA DE PERSONAS (Herencia) ---
class Persona {
public:
    string cedula;
    string nombre;
    string correo;

    Persona(string cedula, string nombre, string correo)
        : cedula(cedula), nombre(nombre), correo(correo) {}
};

class Alumno : public Persona {
public:
    ProgramaAcademico* programa;
    vector<double> notas;

    Alumno(string cedula, string nombre, string correo, ProgramaAcademico* prog, vector<double> n = {0.0, 0.0, 0.0})
        : Persona(cedula, nombre, correo), programa(prog), notas(n) {}
};

class Profesor : public Persona {
public:
    string especialidad;
    string materia;

    Profesor(string cedula, string nombre, string correo, string esp, string mat)
        : Persona(cedula, nombre, correo), especialidad(esp), materia(mat) {}
};

// --- ESTRUCTURA PARA LA PILA (LIFO) ---
struct RegistroNotaHistorial {
    int indiceAlumno;
    int indiceNota;
    double notaAnterior;
};

// --- 3. PERSISTENCIA EN ARCHIVOS .TXT ---
const string ARCHIVO_ALUMNOS = "alumnos.txt";
const string ARCHIVO_PROFESORES = "profesores.txt";
const string ARCHIVO_CERTIFICADOS = "certificados_pendientes.txt";

void guardarTodosAlumnos(const vector<Alumno>& lista) {
    ofstream archivo(ARCHIVO_ALUMNOS);
    if (!archivo.is_open()) {
        cout << "Error al guardar alumnos en disco.\n";
        return;
    }
    for (const auto& a : lista) {
        archivo << a.cedula << ", " << a.nombre << ", " << a.correo << ", "
                << a.programa->nombre << ", " << a.notas[0] << ", "
                << a.notas[1] << ", " << a.notas[2] << "\n";
    }
    archivo.close();
}

vector<Alumno> cargarAlumnos() {
    vector<Alumno> lista;
    ifstream archivo(ARCHIVO_ALUMNOS);
    if (!archivo.is_open()) return lista;

    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string item;
        vector<string> partes;
        while (getline(ss, item, ',')) {
            partes.push_back(trim(item));
        }
        if (partes.size() == 7) {
            ProgramaAcademico* prog = instanciarPrograma(partes[3]);
            if (prog != nullptr) {
                try {
                    vector<double> notas = {
                        stod(partes[4]),
                        stod(partes[5]),
                        stod(partes[6])
                    };
                    lista.push_back(Alumno(partes[0], partes[1], partes[2], prog, notas));
                } catch (...) {
                    delete prog;
                }
            }
        }
    }
    archivo.close();
    return lista;
}

void guardarProfesor(const Profesor& p) {
    ofstream archivo(ARCHIVO_PROFESORES, ios::app);
    if (!archivo.is_open()) {
        cout << "Error al guardar profesor.\n";
        return;
    }
    archivo << p.cedula << ", " << p.nombre << ", " << p.correo << ", "
            << p.especialidad << ", " << p.materia << "\n";
    archivo.close();
}

vector<Profesor> cargarProfesores() {
    vector<Profesor> lista;
    ifstream archivo(ARCHIVO_PROFESORES);
    if (!archivo.is_open()) return lista;

    string linea;
    while (getline(archivo, linea)) {
        stringstream ss(linea);
        string item;
        vector<string> partes;
        while (getline(ss, item, ',')) {
            partes.push_back(trim(item));
        }
        if (partes.size() == 5) {
            lista.push_back(Profesor(partes[0], partes[1], partes[2], partes[3], partes[4]));
        }
    }
    archivo.close();
    return lista;
}

// --- 4. MOTOR CORE Y MENÚ PRINCIPAL ---
int main() {
    vector<Alumno> alumnos = cargarAlumnos();
    vector<Profesor> profesores = cargarProfesores();
    stack<RegistroNotaHistorial> pilaHistorial; // Pila LIFO

    while (true) {
        cout << "\n==============================================\n";
        cout << "      SGA-DO: SISTEMA DIPLOMADOSONLINE        \n";
        cout << "==============================================\n";
        cout << "1. Registrar Alumno\n";
        cout << "2. Registrar Profesor\n";
        cout << "3. Registrar Notas a un Alumno\n";
        cout << "4. Deshacer Ultimo Registro de Nota\n";
        cout << "5. Generar Cola de Certificados\n";
        cout << "6. Mostrar Reporte General\n";
        cout << "7. Salir\n";
        cout << "==============================================\n";
        cout << "Seleccione una opcion (1-7): ";

        string opcion;
        getline(cin, opcion);
        opcion = trim(opcion);

        if (opcion == "1") {
            cout << "\n--- REGISTRAR ALUMNO ---\n";
            cout << "Cedula/ID (ej. V-101): ";
            string cedula;
            getline(cin, cedula);
            cedula = trim(cedula);

            bool existe = false;
            for (const auto& a : alumnos) {
                if (a.cedula == cedula) { existe = true; break; }
            }
            if (existe) {
                cout << "Error: Ya existe un alumno con esa cedula.\n";
                continue;
            }

            cout << "Nombre Completo: ";
            string nombre; getline(cin, nombre);
            cout << "Correo Electronico: ";
            string correo; getline(cin, correo);
            cout << "Programas disponibles: Curso | Diplomado | Bootcamp\n";
            cout << "Tipo de Programa: ";
            string tipoProg; getline(cin, tipoProg);

            ProgramaAcademico* prog = instanciarPrograma(tipoProg);
            if (prog == nullptr) {
                cout << "Error: Programa invalido. Debe escribir Curso, Diplomado o Bootcamp.\n";
                continue;
            }

            alumnos.push_back(Alumno(cedula, trim(nombre), trim(correo), prog));
            guardarTodosAlumnos(alumnos);
            cout << "¡Alumno " << trim(nombre) << " registrado y guardado en " << ARCHIVO_ALUMNOS << "!\n";

        } else if (opcion == "2") {
            cout << "\n--- REGISTRAR PROFESOR ---\n";
            cout << "Cedula/ID (ej. P-505): ";
            string cedula; getline(cin, cedula);
            cedula = trim(cedula);

            bool existe = false;
            for (const auto& p : profesores) {
                if (p.cedula == cedula) { existe = true; break; }
            }
            if (existe) {
                cout << "Error: Ya existe un profesor con esa cedula.\n";
                continue;
            }

            cout << "Nombre Completo: ";
            string nombre; getline(cin, nombre);
            cout << "Correo Electronico: ";
            string correo; getline(cin, correo);
            cout << "Especialidad (ej. Backend, C++): ";
            string esp; getline(cin, esp);
            cout << "Materia Asignada: ";
            string mat; getline(cin, mat);

            Profesor nuevoProf(cedula, trim(nombre), trim(correo), trim(esp), trim(mat));
            profesores.push_back(nuevoProf);
            guardarProfesor(nuevoProf);
            cout << "¡Profesor " << trim(nombre) << " registrado y guardado en " << ARCHIVO_PROFESORES << "!\n";

        } else if (opcion == "3") {
            cout << "\n--- REGISTRAR NOTAS A UN ALUMNO ---\n";
            cout << "Ingrese la Cedula del alumno: ";
            string cedula; getline(cin, cedula);
            cedula = trim(cedula);

            int idxAlumno = -1;
            for (size_t i = 0; i < alumnos.size(); i++) {
                if (alumnos[i].cedula == cedula) { idxAlumno = i; break; }
            }
            if (idxAlumno == -1) {
                cout << "Alumno no encontrado.\n";
                continue;
            }

            cout << "Alumno: " << alumnos[idxAlumno].nombre << " | Notas actuales: ["
                 << alumnos[idxAlumno].notas[0] << ", " << alumnos[idxAlumno].notas[1] << ", "
                 << alumnos[idxAlumno].notas[2] << "]\n";

            try {
                cout << "¿Que evaluacion desea calificar? (1, 2 o 3): ";
                string posStr; getline(cin, posStr);
                int pos = stoi(trim(posStr));
                if (pos < 1 || pos > 3) {
                    cout << "Error: Solo existen evaluaciones 1, 2 o 3.\n";
                    continue;
                }

                cout << "Ingrese la calificacion (0 a 20): ";
                string notaStr; getline(cin, notaStr);
                double nuevaNota = stod(trim(notaStr));
                if (nuevaNota < 0.0 || nuevaNota > 20.0) {
                    cout << "Error: La nota debe estar entre 0 y 20.\n";
                    continue;
                }

                int indice = pos - 1;
                double notaAnterior = alumnos[idxAlumno].notas[indice];
                pilaHistorial.push({idxAlumno, indice, notaAnterior});
                alumnos[idxAlumno].notas[indice] = nuevaNota;
                guardarTodosAlumnos(alumnos);
                cout << "Nota " << pos << " actualizada a " << nuevaNota << " y guardada en disco.\n";

            } catch (...) {
                cout << "Error: Ingrese un valor numerico valido.\n";
            }

        } else if (opcion == "4") {
            cout << "\n--- DESHACER ULTIMO REGISTRO DE NOTA (LIFO) ---\n";
            if (pilaHistorial.empty()) {
                cout << "No hay registros de notas recientes para deshacer.\n";
                continue;
            }
            RegistroNotaHistorial reg = pilaHistorial.top();
            pilaHistorial.pop();

            double notaBorrada = alumnos[reg.indiceAlumno].notas[reg.indiceNota];
            alumnos[reg.indiceAlumno].notas[reg.indiceNota] = reg.notaAnterior;
            guardarTodosAlumnos(alumnos);
            cout << "¡Deshacer exitoso! Se revirtio la Nota " << (reg.indiceNota + 1)
                 << " (" << notaBorrada << " -> " << reg.notaAnterior << ") de "
                 << alumnos[reg.indiceAlumno].nombre << ".\n";

        } else if (opcion == "5") {
            cout << "\n--- GENERAR COLA DE CERTIFICADOS (FIFO) ---\n";
            vector<Alumno> actualizados = cargarAlumnos();
            queue<Alumno> colaCertificados; // Cola FIFO

            for (const auto& a : actualizados) {
                if (a.programa->evaluarAprobacion(a.notas)) {
                    colaCertificados.push(a);
                }
            }

            ofstream archivo(ARCHIVO_CERTIFICADOS);
            if (!archivo.is_open()) {
                cout << "Error al crear archivo de certificados.\n";
                continue;
            }

            archivo << "============================================\n";
            archivo << "REPORTE DE CERTIFICADOS PENDIENTES\n";
            archivo << "============================================\n";
            archivo << "Total de graduandos en cola: " << colaCertificados.size() << "\n\n";

            int contador = 1;
            while (!colaCertificados.empty()) {
                Alumno grad = colaCertificados.front();
                colaCertificados.pop(); // Saca al primero que entro (FIFO)
                double prom = round(grad.programa->calcularPromedio(grad.notas) * 10.0) / 10.0;

                archivo << contador << ". [" << grad.cedula << "] " << grad.nombre << "\n";
                archivo << "   - Programa: " << grad.programa->nombre << "\n";
                archivo << "   - Promedio Final: " << fixed << setprecision(1) << prom << "\n";
                if (grad.programa->nombre == "Bootcamp") {
                    archivo << "   - Estatus: APROBADO (Cumple regla de ninguna nota < 14)\n\n";
                } else {
                    archivo << "   - Estatus: APROBADO\n\n";
                }
                contador++;
            }
            archivo << "============================================\n";
            archivo << "Fin del reporte - Generado por SGA-DO\n";
            archivo.close();

            for (auto& a : actualizados) delete a.programa;
            cout << "¡Cola procesada! Se exporto el archivo '" << ARCHIVO_CERTIFICADOS << "' con exito.\n";

        } else if (opcion == "6") {
            cout << "\n--- REPORTE GENERAL DEL SISTEMA ---\n";
            vector<Profesor> profs = cargarProfesores();
            vector<Alumno> alums = cargarAlumnos();

            cout << "\n[PROFESORES ACTIVOS]\n";
            if (profs.empty()) cout << "No hay profesores registrados.\n";
            for (const auto& p : profs) {
                cout << "- [" << p.cedula << "] " << p.nombre << " | Esp: " << p.especialidad << " | Materia: " << p.materia << "\n";
            }

            cout << "\n[ALUMNOS REGISTRADOS]\n";
            if (alums.empty()) cout << "No hay alumnos registrados.\n";
            for (const auto& a : alums) {
                double prom = round(a.programa->calcularPromedio(a.notas) * 10.0) / 10.0;
                string estatus = a.programa->evaluarAprobacion(a.notas) ? "APROBADO" : "REPROBADO";
                cout << "- [" << a.cedula << "] " << a.nombre << " | " << a.programa->nombre
                     << " | Notas: [" << a.notas[0] << ", " << a.notas[1] << ", " << a.notas[2] << "]"
                     << " | Prom: " << fixed << setprecision(1) << prom << " -> " << estatus << "\n";
            }
            for (auto& a : alums) delete a.programa;

        } else if (opcion == "7") {
            guardarTodosAlumnos(alumnos);
            for (auto& a : alumnos) delete a.programa; // Limpieza de memoria dinámica
            cout << "\nDatos asegurados y memoria limpia. ¡Hasta luego!\n";
            break;

        } else {
            cout << "Error: Ingrese un valor numerico valido entre 1 y 7.\n";
        }
    }
    return 0;
}