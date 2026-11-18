# ========================================================
# SISTEMA DE GESTIÓN ACADÉMICA (SGA-DO)
# Entregable #4: Prototipo Funcional en Python
# Alumna: Rebecca Alexandra Welffer Gutierrez
# ========================================================

import os

# --- 1. JERARQUÍA DE PERSONAS (Herencia) ---
class Persona:
    def __init__(self, cedula, nombre, correo):
        self.cedula = cedula
        self.nombre = nombre
        self.correo = correo

class Alumno(Persona):
    def __init__(self, cedula, nombre, correo, programa, notas=None):
        super().__init__(cedula, nombre, correo)
        self.programa = programa
        self.notas = notas if notas is not None else [0.0, 0.0, 0.0]

    def obtener_notas_validas(self):
        return self.notas

class Profesor(Persona):
    def __init__(self, cedula, nombre, correo, especialidad, materia):
        super().__init__(cedula, nombre, correo)
        self.especialidad = especialidad
        self.materia = materia


# --- 2. JERARQUÍA DE PROGRAMAS ACADÉMICOS (Polimorfismo) ---
class ProgramaAcademico:
    def __init__(self, nombre):
        self.nombre = nombre

    def evaluar_aprobacion(self, notas):
        raise NotImplementedError("Este método debe ser sobrescribido")

    def calcular_promedio(self, notas):
        return sum(notas) / len(notas) if notas else 0.0

class Curso(ProgramaAcademico):
    def __init__(self):
        super().__init__("Curso")

    def evaluar_aprobacion(self, notas):
        return self.calcular_promedio(notas) >= 10.0

class Diplomado(ProgramaAcademico):
    def __init__(self):
        super().__init__("Diplomado")

    def evaluar_aprobacion(self, notas):
        return self.calcular_promedio(notas) >= 14.0

class Bootcamp(ProgramaAcademico):
    def __init__(self):
        super().__init__("Bootcamp")

    def evaluar_aprobacion(self, notas):
        return all(nota >= 14.0 for nota in notas)


# --- 3. MANEJO DE ARCHIVOS (.TXT) Y PERSISTENCIA ---
ARCHIVO_ALUMNOS = "alumnos.txt"
ARCHIVO_PROFESORES = "profesores.txt"
ARCHIVO_CERTIFICADOS = "certificados_pendientes.txt"

def instanciar_programa(nombre_programa):
    nombre = nombre_programa.strip().lower()
    if nombre == "curso":
        return Curso()
    elif nombre == "diplomado":
        return Diplomado()
    elif nombre == "bootcamp":
        return Bootcamp()
    return None

def guardar_todos_alumnos(lista_alumnos):
    with open(ARCHIVO_ALUMNOS, "w", encoding="utf-8") as f:
        for a in lista_alumnos:
            n1, n2, n3 = a.notas[0], a.notas[1], a.notas[2]
            f.write(f"{a.cedula}, {a.nombre}, {a.correo}, {a.programa.nombre}, {n1}, {n2}, {n3}\n")

def cargar_alumnos():
    lista = []
    if not os.path.exists(ARCHIVO_ALUMNOS):
        return lista
    with open(ARCHIVO_ALUMNOS, "r", encoding="utf-8") as f:
        for linea in f:
            partes = [p.strip() for p in linea.strip().split(",")]
            if len(partes) == 7:
                cedula, nombre, correo, prog_str, n1, n2, n3 = partes
                prog_obj = instanciar_programa(prog_str)
                if prog_obj:
                    notas = [float(n1), float(n2), float(n3)]
                    lista.append(Alumno(cedula, nombre, correo, prog_obj, notas))
    return lista

def guardar_profesor(profesor):
    with open(ARCHIVO_PROFESORES, "a", encoding="utf-8") as f:
        f.write(f"{profesor.cedula}, {profesor.nombre}, {profesor.correo}, {profesor.especialidad}, {profesor.materia}\n")

def cargar_profesores():
    lista = []
    if not os.path.exists(ARCHIVO_PROFESORES):
        return lista
    with open(ARCHIVO_PROFESORES, "r", encoding="utf-8") as f:
        for linea in f:
            partes = [p.strip() for p in linea.strip().split(",")]
            if len(partes) == 5:
                lista.append(Profesor(*partes))
    return lista


# --- 4. MOTOR CORE Y MENÚ PRINCIPAL ---
def main():
    alumnos = cargar_alumnos()
    profesores = cargar_profesores()
    pila_historial_notas = []

    while True:
        print("\n==============================================")
        print("      SGA-DO: SISTEMA DIPLOMADOSONLINE        ")
        print("==============================================")
        print("1. Registrar Alumno")
        print("2. Registrar Profesor")
        print("3. Registrar Notas a un Alumno")
        print("4. Deshacer Último Registro de Nota")
        print("5. Generar Cola de Certificados")
        print("6. Mostrar Reporte General")
        print("7. Salir")
        print("==============================================")

        opcion = input("Seleccione una opción (1-7): ").strip()

        if opcion == "1":
            print("\n--- REGISTRAR ALUMNO ---")
            cedula = input("Cédula/ID (ej. V-101): ").strip()
            if any(a.cedula == cedula for a in alumnos):
                print("Error: Ya existe un alumno con esa cédula.")
                continue
            nombre = input("Nombre Completo: ").strip()
            correo = input("Correo Electrónico: ").strip()
            print("Programas disponibles: Curso | Diplomado | Bootcamp")
            prog_input = input("Tipo de Programa: ").strip()
            programa_obj = instanciar_programa(prog_input)

            if not programa_obj:
                print("Error: Programa inválido. Debe escribir Curso, Diplomado o Bootcamp.")
                continue

            nuevo_alumno = Alumno(cedula, nombre, correo, programa_obj)
            alumnos.append(nuevo_alumno)
            guardar_todos_alumnos(alumnos)
            print(f"¡Alumno {nombre} registrado y guardado en {ARCHIVO_ALUMNOS}!")

        elif opcion == "2":
            print("\n--- REGISTRAR PROFESOR ---")
            cedula = input("Cédula/ID (ej. P-505): ").strip()
            if any(p.cedula == cedula for p in profesores):
                print("Error: Ya existe un profesor con esa cédula.")
                continue
            nombre = input("Nombre Completo: ").strip()
            correo = input("Correo Electrónico: ").strip()
            especialidad = input("Especialidad (ej. Backend, Python): ").strip()
            materia = input("Materia Asignada: ").strip()

            nuevo_prof = Profesor(cedula, nombre, correo, especialidad, materia)
            profesores.append(nuevo_prof)
            guardar_profesor(nuevo_prof)
            print(f"¡Profesor {nombre} registrado y guardado en {ARCHIVO_PROFESORES}!")

        elif opcion == "3":
            print("\n--- REGISTRAR NOTAS A UN ALUMNO ---")
            cedula = input("Ingrese la Cédula del alumno: ").strip()
            alumno = next((a for a in alumnos if a.cedula == cedula), None)

            if not alumno:
                print("Alumno no encontrado.")
                continue

            print(f"Alumno: {alumno.nombre} | Notas actuales: {alumno.notas}")
            try:
                pos = int(input("¿Qué evaluación desea calificar? (1, 2 o 3): "))
                if pos not in [1, 2, 3]:
                    print("Error: Solo existen evaluaciones 1, 2 o 3.")
                    continue
                nueva_nota = float(input("Ingrese la calificación (0 a 20): "))
                if not (0 <= nueva_nota <= 20):
                    print("Error: La nota debe estar entre 0 y 20.")
                    continue

                indice = pos - 1
                nota_anterior = alumno.notas[indice]
                pila_historial_notas.append((alumno, indice, nota_anterior))
                alumno.notas[indice] = nueva_nota
                guardar_todos_alumnos(alumnos)
                print(f"Nota {pos} actualizada a {nueva_nota} y guardada en disco.")

            except ValueError:
                print("Error: Ingrese un valor numérico válido.")

        elif opcion == "4":
            print("\n--- DESHACER ÚLTIMO REGISTRO DE NOTA (LIFO) ---")
            if not pila_historial_notas:
                print("No hay registros de notas recientes para deshacer.")
                continue

            alumno, indice, nota_anterior = pila_historial_notas.pop()
            nota_borrada = alumno.notas[indice]
            alumno.notas[indice] = nota_anterior
            guardar_todos_alumnos(alumnos)
            print(f"¡Deshacer exitoso! Se revirtió la Nota {indice + 1} ({nota_borrada} -> {nota_anterior}) de {alumno.nombre}.")

        elif opcion == "5":
            print("\n--- GENERAR COLA DE CERTIFICADOS (FIFO) ---")
            alumnos_actualizados = cargar_alumnos()
            cola_certificados = []

            for a in alumnos_actualizados:
                if a.programa.evaluar_aprobacion(a.notas):
                    cola_certificados.append(a)

            with open(ARCHIVO_CERTIFICADOS, "w", encoding="utf-8") as f:
                f.write("============================================\n")
                f.write("REPORTE DE CERTIFICADOS PENDIENTES\n")
                f.write("============================================\n")
                f.write(f"Total de graduandos en cola: {len(cola_certificados)}\n\n")

                contador = 1
                while cola_certificados:
                    graduando = cola_certificados.pop(0)
                    prom = round(graduando.programa.calcular_promedio(graduando.notas), 1)
                    f.write(f"{contador}. [{graduando.cedula}] {graduando.nombre}\n")
                    f.write(f"   - Programa: {graduando.programa.nombre}\n")
                    f.write(f"   - Promedio Final: {prom}\n")
                    if isinstance(graduando.programa, Bootcamp):
                        f.write("   - Estatus: APROBADO (Cumple regla de ninguna nota < 14)\n\n")
                    else:
                        f.write("   - Estatus: APROBADO\n\n")
                    contador += 1

                f.write("============================================\n")
                f.write("Fin del reporte - Generado por SGA-DO\n")

            print(f"¡Cola procesada! Se exportó el archivo '{ARCHIVO_CERTIFICADOS}' con éxito.")

        elif opcion == "6":
            print("\n--- REPORTE GENERAL DEL SISTEMA ---")
            profs = cargar_profesores()
            alums = cargar_alumnos()

            print("\n[PROFESORES ACTIVOS]")
            if not profs:
                print("No hay profesores registrados.")
            for p in profs:
                print(f"- [{p.cedula}] {p.nombre} | Esp: {p.especialidad} | Materia: {p.materia}")

            print("\n[ALUMNOS REGISTRADOS]")
            if not alums:
                print("No hay alumnos registrados.")
            for a in alums:
                prom = round(a.programa.calcular_promedio(a.notas), 1)
                estatus = "APROBADO" if a.programa.evaluar_aprobacion(a.notas) else "REPROBADO"
                print(f"- [{a.cedula}] {a.nombre} | {a.programa.nombre} | Notas: {a.notas} | Prom: {prom} -> {estatus}")

        elif opcion == "7":
            guardar_todos_alumnos(alumnos)
            pila_historial_notas.clear()
            print("\nDatos asegurados y memoria limpia. ¡Hasta luego!")
            break

        else:
            print("Error: Ingrese un valor numérico válido entre 1 y 7.")

if __name__ == "__main__":
    main()