// ========================================================
// SISTEMA DE GESTIÓN ACADÉMICA (SGA-DO)
// Entregable #5: Prototipo Funcional en Java
// Alumna: Rebecca Alexandra Welffer Gutierrez
// ========================================================

import java.io.*;
import java.util.*;

// --- 1. JERARQUÍA DE PERSONAS (Herencia) ---
class Persona {
    protected String cedula;
    protected String nombre;
    protected String correo;

    public Persona(String cedula, String nombre, String correo) {
        this.cedula = cedula;
        this.nombre = nombre;
        this.correo = correo;
    }
}

class Alumno extends Persona {
    protected ProgramaAcademico programa;
    protected double[] notas;

    public Alumno(String cedula, String nombre, String correo, ProgramaAcademico programa, double[] notas) {
        super(cedula, nombre, correo);
        this.programa = programa;
        this.notas = (notas != null) ? notas : new double[]{0.0, 0.0, 0.0};
    }
}

class Profesor extends Persona {
    protected String especialidad;
    protected String materia;

    public Profesor(String cedula, String nombre, String correo, String especialidad, String materia) {
        super(cedula, nombre, correo);
        this.especialidad = especialidad;
        this.materia = materia;
    }
}

// --- 2. JERARQUÍA DE PROGRAMAS ACADÉMICOS (Polimorfismo) ---
abstract class ProgramaAcademico {
    protected String nombre;

    public ProgramaAcademico(String nombre) {
        this.nombre = nombre;
    }

    public abstract boolean evaluarAprobacion(double[] notas);

    public double calcularPromedio(double[] notas) {
        if (notas == null || notas.length == 0) return 0.0;
        double suma = 0;
        for (double n : notas) suma += n;
        return suma / notas.length;
    }
}

class Curso extends ProgramaAcademico {
    public Curso() { super("Curso"); }

    @Override
    public boolean evaluarAprobacion(double[] notas) {
        return calcularPromedio(notas) >= 10.0;
    }
}

class Diplomado extends ProgramaAcademico {
    public Diplomado() { super("Diplomado"); }

    @Override
    public boolean evaluarAprobacion(double[] notas) {
        return calcularPromedio(notas) >= 14.0;
    }
}

class Bootcamp extends ProgramaAcademico {
    public Bootcamp() { super("Bootcamp"); }

    @Override
    public boolean evaluarAprobacion(double[] notas) {
        for (double n : notas) {
            if (n < 14.0) return false;
        }
        return true;
    }
}

// --- CLASE AUXILIAR PARA LA PILA (LIFO) ---
class RegistroNotaHistorial {
    Alumno alumno;
    int indice;
    double notaAnterior;

    public RegistroNotaHistorial(Alumno alumno, int indice, double notaAnterior) {
        this.alumno = alumno;
        this.indice = indice;
        this.notaAnterior = notaAnterior;
    }
}

// --- 3 y 4. MOTOR CORE, PERSISTENCIA Y MENÚ PRINCIPAL ---
public class Main {
    static final String ARCHIVO_ALUMNOS = "alumnos.txt";
    static final String ARCHIVO_PROFESORES = "profesores.txt";
    static final String ARCHIVO_CERTIFICADOS = "certificados_pendientes.txt";

    public static ProgramaAcademico instanciarPrograma(String nombre) {
        String p = nombre.trim().toLowerCase();
        if (p.equals("curso")) return new Curso();
        if (p.equals("diplomado")) return new Diplomado();
        if (p.equals("bootcamp")) return new Bootcamp();
        return null;
    }

    public static void guardarTodosAlumnos(List<Alumno> lista) {
        try (PrintWriter pw = new PrintWriter(new FileWriter(ARCHIVO_ALUMNOS))) {
            for (Alumno a : lista) {
                pw.println(a.cedula + ", " + a.nombre + ", " + a.correo + ", " +
                           a.programa.nombre + ", " + a.notas[0] + ", " + a.notas[1] + ", " + a.notas[2]);
            }
        } catch (IOException e) {
            System.out.println("Error al guardar alumnos: " + e.getMessage());
        }
    }

    public static List<Alumno> cargarAlumnos() {
        List<Alumno> lista = new ArrayList<>();
        File file = new File(ARCHIVO_ALUMNOS);
        if (!file.exists()) return lista;

        try (BufferedReader br = new BufferedReader(new FileReader(file))) {
            String linea;
            while ((linea = br.readLine()) != null) {
                String[] partes = linea.split(",");
                if (partes.length == 7) {
                    String cedula = partes[0].trim();
                    String nombre = partes[1].trim();
                    String correo = partes[2].trim();
                    ProgramaAcademico prog = instanciarPrograma(partes[3].trim());
                    if (prog != null) {
                        double[] notas = {
                            Double.parseDouble(partes[4].trim()),
                            Double.parseDouble(partes[5].trim()),
                            Double.parseDouble(partes[6].trim())
                        };
                        lista.add(new Alumno(cedula, nombre, correo, prog, notas));
                    }
                }
            }
        } catch (Exception e) {
            System.out.println("Error al leer alumnos: " + e.getMessage());
        }
        return lista;
    }

    public static void guardarProfesor(Profesor p) {
        try (PrintWriter pw = new PrintWriter(new FileWriter(ARCHIVO_PROFESORES, true))) {
            pw.println(p.cedula + ", " + p.nombre + ", " + p.correo + ", " + p.especialidad + ", " + p.materia);
        } catch (IOException e) {
            System.out.println("Error al guardar profesor: " + e.getMessage());
        }
    }

    public static List<Profesor> cargarProfesores() {
        List<Profesor> lista = new ArrayList<>();
        File file = new File(ARCHIVO_PROFESORES);
        if (!file.exists()) return lista;

        try (BufferedReader br = new BufferedReader(new FileReader(file))) {
            String linea;
            while ((linea = br.readLine()) != null) {
                String[] partes = linea.split(",");
                if (partes.length == 5) {
                    lista.add(new Profesor(partes[0].trim(), partes[1].trim(), partes[2].trim(),
                                           partes[3].trim(), partes[4].trim()));
                }
            }
        } catch (Exception e) {
            System.out.println("Error al leer profesores: " + e.getMessage());
        }
        return lista;
    }

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        List<Alumno> alumnos = cargarAlumnos();
        List<Profesor> profesores = cargarProfesores();
        Stack<RegistroNotaHistorial> pilaHistorial = new Stack<>(); // Pila LIFO

        while (true) {
            System.out.println("\n==============================================");
            System.out.println("      SGA-DO: SISTEMA DIPLOMADOSONLINE        ");
            System.out.println("==============================================");
            System.out.println("1. Registrar Alumno");
            System.out.println("2. Registrar Profesor");
            System.out.println("3. Registrar Notas a un Alumno");
            System.out.println("4. Deshacer Último Registro de Nota");
            System.out.println("5. Generar Cola de Certificados");
            System.out.println("6. Mostrar Reporte General");
            System.out.println("7. Salir");
            System.out.println("==============================================");
            System.out.print("Seleccione una opción (1-7): ");

            String opcion = sc.nextLine().trim();

            if (opcion.equals("1")) {
                System.out.println("\n--- REGISTRAR ALUMNO ---");
                System.out.print("Cédula/ID (ej. V-101): ");
                String cedula = sc.nextLine().trim();
                boolean existe = false;
                for (Alumno a : alumnos) {
                    if (a.cedula.equals(cedula)) { existe = true; break; }
                }
                if (existe) {
                    System.out.println("Error: Ya existe un alumno con esa cédula.");
                    continue;
                }
                System.out.print("Nombre Completo: ");
                String nombre = sc.nextLine().trim();
                System.out.print("Correo Electrónico: ");
                String correo = sc.nextLine().trim();
                System.out.println("Programas disponibles: Curso | Diplomado | Bootcamp");
                System.out.print("Tipo de Programa: ");
                ProgramaAcademico prog = instanciarPrograma(sc.nextLine());

                if (prog == null) {
                    System.out.println("Error: Programa inválido. Debe escribir Curso, Diplomado o Bootcamp.");
                    continue;
                }

                alumnos.add(new Alumno(cedula, nombre, correo, prog, null));
                guardarTodosAlumnos(alumnos);
                System.out.println("¡Alumno " + nombre + " registrado y guardado en " + ARCHIVO_ALUMNOS + "!");

            } else if (opcion.equals("2")) {
                System.out.println("\n--- REGISTRAR PROFESOR ---");
                System.out.print("Cédula/ID (ej. P-505): ");
                String cedula = sc.nextLine().trim();
                boolean existe = false;
                for (Profesor p : profesores) {
                    if (p.cedula.equals(cedula)) { existe = true; break; }
                }
                if (existe) {
                    System.out.println("Error: Ya existe un profesor con esa cédula.");
                    continue;
                }
                System.out.print("Nombre Completo: ");
                String nombre = sc.nextLine().trim();
                System.out.print("Correo Electrónico: ");
                String correo = sc.nextLine().trim();
                System.out.print("Especialidad (ej. Backend, Java): ");
                String esp = sc.nextLine().trim();
                System.out.print("Materia Asignada: ");
                String mat = sc.nextLine().trim();

                Profesor nuevoProf = new Profesor(cedula, nombre, correo, esp, mat);
                profesores.add(nuevoProf);
                guardarProfesor(nuevoProf);
                System.out.println("¡Profesor " + nombre + " registrado y guardado en " + ARCHIVO_PROFESORES + "!");

            } else if (opcion.equals("3")) {
                System.out.println("\n--- REGISTRAR NOTAS A UN ALUMNO ---");
                System.out.print("Ingrese la Cédula del alumno: ");
                String cedula = sc.nextLine().trim();
                Alumno encontrado = null;
                for (Alumno a : alumnos) {
                    if (a.cedula.equals(cedula)) { encontrado = a; break; }
                }
                if (encontrado == null) {
                    System.out.println("Alumno no encontrado.");
                    continue;
                }

                System.out.println("Alumno: " + encontrado.nombre + " | Notas actuales: " + Arrays.toString(encontrado.notas));
                try {
                    System.out.print("¿Qué evaluación desea calificar? (1, 2 o 3): ");
                    int pos = Integer.parseInt(sc.nextLine().trim());
                    if (pos < 1 || pos > 3) {
                        System.out.println("Error: Solo existen evaluaciones 1, 2 o 3.");
                        continue;
                    }
                    System.out.print("Ingrese la calificación (0 a 20): ");
                    double nuevaNota = Double.parseDouble(sc.nextLine().trim());
                    if (nuevaNota < 0 || nuevaNota > 20) {
                        System.out.println("Error: La nota debe estar entre 0 y 20.");
                        continue;
                    }

                    int indice = pos - 1;
                    double notaAnterior = encontrado.notas[indice];
                    pilaHistorial.push(new RegistroNotaHistorial(encontrado, indice, notaAnterior));
                    encontrado.notas[indice] = nuevaNota;
                    guardarTodosAlumnos(alumnos);
                    System.out.println("Nota " + pos + " actualizada a " + nuevaNota + " y guardada en disco.");

                } catch (NumberFormatException e) {
                    System.out.println("Error: Ingrese un valor numérico válido.");
                }

            } else if (opcion.equals("4")) {
                System.out.println("\n--- DESHACER ÚLTIMO REGISTRO DE NOTA (LIFO) ---");
                if (pilaHistorial.isEmpty()) {
                    System.out.println("No hay registros de notas recientes para deshacer.");
                    continue;
                }
                RegistroNotaHistorial reg = pilaHistorial.pop();
                double notaBorrada = reg.alumno.notas[reg.indice];
                reg.alumno.notas[reg.indice] = reg.notaAnterior;
                guardarTodosAlumnos(alumnos);
                System.out.println("¡Deshacer exitoso! Se revirtió la Nota " + (reg.indice + 1) +
                                   " (" + notaBorrada + " -> " + reg.notaAnterior + ") de " + reg.alumno.nombre + ".");

            } else if (opcion.equals("5")) {
                System.out.println("\n--- GENERAR COLA DE CERTIFICADOS (FIFO) ---");
                List<Alumno> actualizados = cargarAlumnos();
                Queue<Alumno> colaCertificados = new LinkedList<>(); // Cola FIFO

                for (Alumno a : actualizados) {
                    if (a.programa.evaluarAprobacion(a.notas)) {
                        colaCertificados.offer(a);
                    }
                }

                try (PrintWriter pw = new PrintWriter(new FileWriter(ARCHIVO_CERTIFICADOS))) {
                    pw.println("============================================");
                    pw.println("REPORTE DE CERTIFICADOS PENDIENTES");
                    pw.println("============================================");
                    pw.println("Total de graduandos en cola: " + colaCertificados.size() + "\n");

                    int contador = 1;
                    while (!colaCertificados.isEmpty()) {
                        Alumno grad = colaCertificados.poll(); // Saca al primero que entró (FIFO)
                        double prom = Math.round(grad.programa.calcularPromedio(grad.notas) * 10.0) / 10.0;
                        pw.println(contador + ". [" + grad.cedula + "] " + grad.nombre);
                        pw.println("   - Programa: " + grad.programa.nombre);
                        pw.println("   - Promedio Final: " + prom);
                        if (grad.programa instanceof Bootcamp) {
                            pw.println("   - Estatus: APROBADO (Cumple regla de ninguna nota < 14)\n");
                        } else {
                            pw.println("   - Estatus: APROBADO\n");
                        }
                        contador++;
                    }
                    pw.println("============================================");
                    pw.println("Fin del reporte - Generado por SGA-DO");
                    System.out.println("¡Cola procesada! Se exportó el archivo '" + ARCHIVO_CERTIFICADOS + "' con éxito.");
                } catch (IOException e) {
                    System.out.println("Error al generar certificados: " + e.getMessage());
                }

            } else if (opcion.equals("6")) {
                System.out.println("\n--- REPORTE GENERAL DEL SISTEMA ---");
                List<Profesor> profs = cargarProfesores();
                List<Alumno> alums = cargarAlumnos();

                System.out.println("\n[PROFESORES ACTIVOS]");
                if (profs.isEmpty()) System.out.println("No hay profesores registrados.");
                for (Profesor p : profs) {
                    System.out.println("- [" + p.cedula + "] " + p.nombre + " | Esp: " + p.especialidad + " | Materia: " + p.materia);
                }

                System.out.println("\n[ALUMNOS REGISTRADOS]");
                if (alums.isEmpty()) System.out.println("No hay alumnos registrados.");
                for (Alumno a : alums) {
                    double prom = Math.round(a.programa.calcularPromedio(a.notas) * 10.0) / 10.0;
                    String estatus = a.programa.evaluarAprobacion(a.notas) ? "APROBADO" : "REPROBADO";
                    System.out.println("- [" + a.cedula + "] " + a.nombre + " | " + a.programa.nombre +
                                       " | Notas: " + Arrays.toString(a.notas) + " | Prom: " + prom + " -> " + estatus);
                }

            } else if (opcion.equals("7")) {
                guardarTodosAlumnos(alumnos);
                pilaHistorial.clear();
                System.out.println("\nDatos asegurados y memoria limpia. ¡Hasta luego!");
                break;

            } else {
                System.out.println("Error: Ingrese un valor numérico válido entre 1 y 7.");
            }
        }
        sc.close();
    }
}