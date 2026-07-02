#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;
using namespace std::chrono;

struct Transaccion {
  string idTransaccion;
  string cuentaOrigen;
  string cliente;
  string tipo;
  double monto;
  string fecha;
  string hora;
  string estado;
  string fechaHoraOrden;

  Transaccion() : monto(0.0) {}

  Transaccion(string id, string cuenta, string cli, string tip, double mon,
              string fec, string hor, string est) {
    idTransaccion = id;
    cuentaOrigen = cuenta;
    cliente = cli;
    tipo = tip;
    monto = mon;
    fecha = fec;
    hora = hor;
    estado = est;
    fechaHoraOrden = fec + " " + hor;
  }

  void mostrar() const {
    cout << "  ID          : " << idTransaccion << "\n"
         << "  Cuenta      : " << cuentaOrigen << "\n"
         << "  Cliente     : " << cliente << "\n"
         << "  Tipo        : " << tipo << "\n"
         << "  Monto       : " << monto << "\n"
         << "  Fecha       : " << fecha << "\n"
         << "  Hora        : " << hora << "\n"
         << "  Estado      : " << estado << "\n";
  }
};

// Estructura para guardar clave-valor
struct Registro {
  string clave;
  int cantidad;
};
// Contenedor de registros (por tipo de transaccion y por estado de transaccion)
struct ContenedorRegistros {
  Registro *datos;
  int tamanio;
  int capacidad;

  ContenedorRegistros() {
    capacidad = 10; // Inicia con 10 espacios
    datos = new Registro[capacidad];
    tamanio = 0;
  }

  ~ContenedorRegistros() { delete[] datos; }

  // Busca si la clave existe, retorna índice o -1
  int buscarIndice(const string &clave) {
    for (int i = 0; i < tamanio; i++) {
      if (datos[i].clave == clave) {
        return i;
      }
    }
    return -1;
  }

  // Incrementa cantidad de una clave
  void incrementar(const string &clave) {
    int idx = buscarIndice(clave);

    if (idx != -1) {
      // Clave existe, incrementar
      datos[idx].cantidad++;
    } else {
      // Clave nueva, agregar
      if (tamanio >= capacidad) {
        // Expandir array si es necesario
        capacidad *= 2;
        Registro *temp = new Registro[capacidad];
        for (int i = 0; i < tamanio; i++) {
          temp[i] = datos[i];
        }
        delete[] datos;
        datos = temp;
      }
      datos[tamanio].clave = clave;
      datos[tamanio].cantidad = 1;
      tamanio++;
    }
  }
};

// Nueva estructura para guardar las estadisticas generales
struct Estadisticas {
  int TotalTransacciones;
  double montoTotal;
  double montoPromedio;
  double mayorMonto;
  double menorMonto;
  Transaccion *trMayor;
  Transaccion *trMenor;

  ContenedorRegistros porTipo;
  ContenedorRegistros porEstado;

  void mostrar() const {
    cout << "\n------------------------------------------\n"
         << " ESTADISTICAS GENERALES\n"
         << "--------------------------------------------\n";

    cout << " Total de transacciones: " << TotalTransacciones << "\n";
    cout << " Monto total: $" << fixed << setprecision(2) << montoTotal << "\n";
    cout << " Monto promedio: $" << montoPromedio << "\n";
    cout << " Mayor monto: $" << mayorMonto << "\n";
    cout << " ID: " << trMayor->idTransaccion
         << " | Cliente: " << trMayor->cliente << "\n";
    cout << " Menor monto: $" << menorMonto << "\n";
    cout << " ID: " << trMenor->idTransaccion
         << " | Cliente: " << trMenor->cliente << "\n";

    cout << "\n--- Transacciones por TIPO ---\n";
    for (int i = 0; i < porTipo.tamanio; i++) {
      cout << "  " << porTipo.datos[i].clave << ": "
           << porTipo.datos[i].cantidad << "\n";
    }

    cout << "\n--- Transacciones por ESTADO ---\n";
    for (int i = 0; i < porEstado.tamanio; i++) {
      cout << "  " << porEstado.datos[i].clave << ": "
           << porEstado.datos[i].cantidad << "\n";
    }
  }
};

struct Tupla {
  Transaccion *dato;
  Tupla *siguiente;

  Tupla(Transaccion *t) : dato(t), siguiente(nullptr) {}
  void setNext(Tupla *tmp) { siguiente = tmp; }
  Tupla *getNext() { return siguiente; }
  Transaccion *getValue() { return dato; }
};

class hashTable {
private:
  Tupla **table;
  int size;

  // Hash para strings:
  int hashFunction(const string &key) {
    long long h = 0;
    for (int i = 0; i < (int)key.size(); i++)
      h = (h * 31 + (unsigned char)key[i]) % size;
    return (int)h;
  }

public:
  hashTable(int s) {
    size = s;
    table = new Tupla *[size];
    for (int i = 0; i < size; i++) {
      table[i] = nullptr;
    }
  }

  ~hashTable() {
    for (int i = 0; i < size; i++) {
      Tupla *actual = table[i];
      while (actual != nullptr) {
        Tupla *temp = actual;
        actual = actual->siguiente;
        delete temp; // Libera cada tupla
      }
    }
    delete[] table;
  }

  void insert(Transaccion *t) {
    int pos = hashFunction(t->idTransaccion);
    Tupla *nueva = new Tupla(t);
    // Insertar al inicio del bucket (más eficiente que al final)
    nueva->siguiente = table[pos];
    table[pos] = nueva;
  }

  Transaccion *buscar(const string &id) {
    int pos = hashFunction(id);
    Tupla *actual = table[pos];

    while (actual != nullptr) {
      if (actual->dato->idTransaccion == id)
        return actual->dato;
      actual = actual->siguiente;
    }
    return nullptr;
  }

  void print() {
    for (int i = 0; i < size; i++) {
      cout << "Indice " << i << ": ";
      Tupla *actual = table[i];
      if (actual == nullptr) {
        cout << "Vacio";
      } else {
        while (actual != nullptr) {
          cout << actual->dato->idTransaccion << " -> ";
          actual = actual->siguiente;
        }
      }
      cout << "\n";
    }
  }

  // Calcula el indice hash del ID y manda a realizar la eliminacion
  bool eliminar(const string &id) {
    int pos = hashFunction(id);
    Tupla *actual = table[pos];

    // Si es el primero
    if (actual != nullptr && actual->dato->idTransaccion == id) {
      table[pos] = actual->siguiente;
      delete actual;
      return true;
    }

    // Buscar en el resto
    while (actual != nullptr) {
      if (actual->siguiente != nullptr &&
          actual->siguiente->dato->idTransaccion == id) {
        Tupla *aEliminar = actual->siguiente;
        actual->siguiente = aEliminar->siguiente;
        delete aEliminar;
        return true;
      }
      actual = actual->siguiente;
    }
    return false;
  }

  // Devuelve estadisticas generales del sistema de transaccion
  Estadisticas calcularEstadisticas() {
    Estadisticas stats;

    stats.TotalTransacciones = 0;
    stats.montoTotal = 0;
    stats.montoPromedio = 0;
    stats.mayorMonto = -1;
    stats.menorMonto = 999999999;
    stats.trMayor = nullptr;
    stats.trMenor = nullptr;

    // Recorrer cada bucket
    for (int i = 0; i < size; i++) {
      Tupla *actual = table[i];
      while (actual != nullptr) {
        Transaccion *t = actual->dato;

        // Contar total de transacciones
        stats.TotalTransacciones++;

        // Sumar montos de cada transaccion
        stats.montoTotal += t->monto;

        // Obtencion del mayor monto
        if (t->monto > stats.mayorMonto) {
          stats.mayorMonto = t->monto;
          stats.trMayor = t;
        }

        // Obtencion del menor monto
        if (t->monto < stats.menorMonto) {
          stats.menorMonto = t->monto;
          stats.trMenor = t;
        }

        // Contar por tipo
        stats.porTipo.incrementar(t->tipo);

        // Contar por estado
        stats.porEstado.incrementar(t->estado);

        actual = actual->getNext();
      }
    }

    // Calcular promedio
    if (stats.TotalTransacciones > 0) {
      stats.montoPromedio = stats.montoTotal / stats.TotalTransacciones;
    }

    return stats;
  }
};

enum Color { ROJO, NEGRO };

class RBNode {
public:
  Transaccion *dato;
  Color color;
  RBNode *izquierda, *derecha, *padre;

  RBNode(Transaccion *t) {
    dato = t;
    color = ROJO;
    izquierda = derecha = padre = nullptr;
  }
};

class RedBlackTree {
private:
  RBNode *raiz;

  void rotarIzquierda(RBNode *&raiz, RBNode *&x) {
    RBNode *y = x->derecha;
    x->derecha = y->izquierda;
    if (y->izquierda != nullptr)
      y->izquierda->padre = x;
    y->padre = x->padre;
    if (x->padre == nullptr)
      raiz = y;
    else if (x == x->padre->izquierda)
      x->padre->izquierda = y;
    else
      x->padre->derecha = y;
    y->izquierda = x;
    x->padre = y;
  }

  void rotarDerecha(RBNode *&raiz, RBNode *&y) {
    RBNode *x = y->izquierda;
    y->izquierda = x->derecha;
    if (x->derecha != nullptr)
      x->derecha->padre = y;
    x->padre = y->padre;
    if (y->padre == nullptr)
      raiz = x;
    else if (y == y->padre->izquierda)
      y->padre->izquierda = x;
    else
      y->padre->derecha = x;
    x->derecha = y;
    y->padre = x;
  }

  void fixInsercion(RBNode *&raiz, RBNode *&nodo) {
    RBNode *padre = nullptr, *abuelo = nullptr;
    while (nodo != raiz && nodo->color == ROJO && nodo->padre->color == ROJO) {
      padre = nodo->padre;
      abuelo = padre->padre;
      if (padre == abuelo->izquierda) {
        RBNode *tio = abuelo->derecha;
        if (tio != nullptr && tio->color == ROJO) {
          abuelo->color = ROJO;
          padre->color = NEGRO;
          tio->color = NEGRO;
          nodo = abuelo;
        } else {
          if (nodo == padre->derecha) {
            rotarIzquierda(raiz, padre);
            nodo = padre;
            padre = nodo->padre;
          }
          rotarDerecha(raiz, abuelo);
          swap(padre->color, abuelo->color);
          nodo = padre;
        }
      } else {
        RBNode *tio = abuelo->izquierda;
        if (tio != nullptr && tio->color == ROJO) {
          abuelo->color = ROJO;
          padre->color = NEGRO;
          tio->color = NEGRO;
          nodo = abuelo;
        } else {
          if (nodo == padre->izquierda) {
            rotarDerecha(raiz, padre);
            nodo = padre;
            padre = nodo->padre;
          }
          rotarIzquierda(raiz, abuelo);
          swap(padre->color, abuelo->color);
          nodo = padre;
        }
      }
    }
    raiz->color = NEGRO;
  }

  void insertarNodo(RBNode *&raiz, RBNode *&nodo) {
    if (raiz == nullptr) {
      raiz = nodo;
      return;
    }
    if (nodo->dato->fechaHoraOrden < raiz->dato->fechaHoraOrden) {
      insertarNodo(raiz->izquierda, nodo);
      raiz->izquierda->padre = raiz;
    } else {
      insertarNodo(raiz->derecha, nodo);
      raiz->derecha->padre = raiz;
    }
  }

  void inorden(RBNode *nodo) {
    if (nodo == nullptr)
      return;
    inorden(nodo->izquierda);
    cout << nodo->dato->fechaHoraOrden << " | " << nodo->dato->idTransaccion
         << " | " << (nodo->color == ROJO ? "R" : "N") << "\n";
    inorden(nodo->derecha);
  }

  void inordenLimitado(RBNode *nodo, int limite, int &mostrados) {
    if (nodo == nullptr || mostrados >= limite)
      return;
    inordenLimitado(nodo->izquierda, limite, mostrados);
    if (mostrados >= limite)
      return; // ya se completaron las N
    mostrados++;
    cout << "  [" << mostrados << "] " << nodo->dato->fechaHoraOrden << " | "
         << nodo->dato->idTransaccion << " | " << nodo->dato->cliente << " | "
         << nodo->dato->tipo << " | " << nodo->dato->monto << " | "
         << nodo->dato->estado << "\n";
    inordenLimitado(nodo->derecha, limite, mostrados);
  }

  // Consulta por rango de fecha "chefcinho"
  void consulRanFech(RBNode *nodo, const string &fechaInicio,
                     const string &fechaFin, int &totalEncontradas,
                     int limiteMuestra) {
    if (nodo == nullptr)
      return;
    if (nodo->dato->fechaHoraOrden >= fechaInicio) {
      consulRanFech(nodo->izquierda, fechaInicio, fechaFin, totalEncontradas,
                    limiteMuestra);
    }
    if (nodo->dato->fechaHoraOrden >= fechaInicio &&
        nodo->dato->fechaHoraOrden <= fechaFin) {

      totalEncontradas++;

      if (totalEncontradas <= limiteMuestra) {
        cout << "  [" << totalEncontradas << "] " << nodo->dato->fechaHoraOrden
             << " | " << nodo->dato->idTransaccion << " | "
             << nodo->dato->cliente << " | " << nodo->dato->tipo << " | $"
             << nodo->dato->monto << " | " << nodo->dato->estado << "\n";
      }
    }
    if (nodo->dato->fechaHoraOrden <= fechaFin) {
      consulRanFech(nodo->derecha, fechaInicio, fechaFin, totalEncontradas,
                    limiteMuestra);
    }
  }

  int contarNodos(RBNode *nodo) {
    if (nodo == nullptr)
      return 0;
    return 1 + contarNodos(nodo->izquierda) + contarNodos(nodo->derecha);
  }

  // Eliminacion
  //  Busca el nodo con el ID
  RBNode *buscarNodo(RBNode *nodo, const string &id) {
    if (nodo == nullptr)
      return nullptr;
    if (nodo->dato->idTransaccion == id)
      return nodo;

    RBNode *izq = buscarNodo(nodo->izquierda, id);
    if (izq != nullptr)
      return izq;
    return buscarNodo(nodo->derecha, id);
  }

  // Encuentra el sucesor inorden (nodo minimo del sub arbol derecho)
  RBNode *minimo(RBNode *nodo) {
    while (nodo->izquierda != nullptr)
      nodo = nodo->izquierda;
    return nodo;
  }

  // Reemplaza un subarbol
  void trasplantar(RBNode *&raiz, RBNode *u, RBNode *v) {
    if (u->padre == nullptr)
      raiz = v;
    else if (u == u->padre->izquierda)
      u->padre->izquierda = v;
    else
      u->padre->derecha = v;

    if (v != nullptr)
      v->padre = u->padre;
  }

  // Arregla violaciones de color despues de eliminar
  void fixEliminacion(RBNode *&raiz, RBNode *nodo) {
    while (nodo != raiz && color(nodo) == NEGRO) {
      if (nodo == nodo->padre->izquierda) {
        RBNode *hermano = nodo->padre->derecha;

        // Caso 1: hermano es rojo
        if (color(hermano) == ROJO) {
          hermano->color = NEGRO;
          nodo->padre->color = ROJO;
          rotarIzquierda(raiz, nodo->padre);
          hermano = nodo->padre->derecha;
        }

        // Caso 2: hermano y sus hijos son negros
        if (color(hermano->izquierda) == NEGRO &&
            color(hermano->derecha) == NEGRO) {
          hermano->color = ROJO;
          nodo = nodo->padre;
        } else {
          // Caso 3: hijo derecho del hermano es negro
          if (color(hermano->derecha) == NEGRO) {
            if (hermano->izquierda != nullptr)
              hermano->izquierda->color = NEGRO;
            hermano->color = ROJO;
            rotarDerecha(raiz, hermano);
            hermano = nodo->padre->derecha;
          }
          // Caso 4: hijo derecho del hermano es rojo
          hermano->color = nodo->padre->color;
          nodo->padre->color = NEGRO;
          if (hermano->derecha != nullptr)
            hermano->derecha->color = NEGRO;
          rotarIzquierda(raiz, nodo->padre);
          nodo = raiz;
        }
      } else {
        RBNode *hermano = nodo->padre->izquierda;

        // Caso 1: hermano es rojo
        if (color(hermano) == ROJO) {
          hermano->color = NEGRO;
          nodo->padre->color = ROJO;
          rotarDerecha(raiz, nodo->padre);
          hermano = nodo->padre->izquierda;
        }

        // Caso 2: hermano y sus hijos son negros
        if (color(hermano->derecha) == NEGRO &&
            color(hermano->izquierda) == NEGRO) {
          hermano->color = ROJO;
          nodo = nodo->padre;
        } else {
          // Caso 3: hijo izquierdo del hermano es negro
          if (color(hermano->izquierda) == NEGRO) {
            if (hermano->derecha != nullptr)
              hermano->derecha->color = NEGRO;
            hermano->color = ROJO;
            rotarIzquierda(raiz, hermano);
            hermano = nodo->padre->izquierda;
          }
          // Caso 4: hijo izquierdo del hermano es rojo
          hermano->color = nodo->padre->color;
          nodo->padre->color = NEGRO;
          if (hermano->izquierda != nullptr)
            hermano->izquierda->color = NEGRO;
          rotarDerecha(raiz, nodo->padre);
          nodo = raiz;
        }
      }
    }
    if (nodo != nullptr)
      nodo->color = NEGRO;
  }

  // Funcion auxiliar para obtener el color de un nodo (nullptr es NEGRO)
  Color color(RBNode *nodo) { return (nodo == nullptr) ? NEGRO : nodo->color; }

  // Elimina recursivamente
  void eliminarNodo(RBNode *&raiz, const string &id) {
    RBNode *nodo = buscarNodo(raiz, id);
    if (nodo == nullptr)
      return;

    RBNode *nodoAArreglar = nullptr;
    RBNode *padreAArreglar = nullptr;
    Color colorOriginal = nodo->color;

    // Caso 1: nodo no tiene hijo izquierdo
    if (nodo->izquierda == nullptr) {
      nodoAArreglar = nodo->derecha;
      padreAArreglar = nodo->padre;
      trasplantar(raiz, nodo, nodo->derecha);
    }
    // Caso 2: nodo no tiene hijo derecho
    else if (nodo->derecha == nullptr) {
      nodoAArreglar = nodo->izquierda;
      padreAArreglar = nodo->padre;
      trasplantar(raiz, nodo, nodo->izquierda);
    }
    // Caso 3: nodo tiene ambos hijos
    else {
      RBNode *sucesor = minimo(nodo->derecha);
      colorOriginal = sucesor->color;
      nodoAArreglar = sucesor->derecha;

      if (sucesor->padre == nodo) {
        padreAArreglar = sucesor;
      } else {
        padreAArreglar = sucesor->padre;
        trasplantar(raiz, sucesor, sucesor->derecha);
        sucesor->derecha = nodo->derecha;
        sucesor->derecha->padre = sucesor;
      }
      trasplantar(raiz, nodo, sucesor);
      sucesor->izquierda = nodo->izquierda;
      sucesor->izquierda->padre = sucesor;
      sucesor->color = nodo->color;
    }

    delete nodo;

    // Rebalancear si se elimina un nodo negro
    if (colorOriginal == NEGRO && nodoAArreglar != nullptr) {
      fixEliminacion(raiz, nodoAArreglar);
    }
  }

public:
  RedBlackTree() { raiz = nullptr; }

  void insertar(Transaccion *t) {
    RBNode *nuevo = new RBNode(t);
    insertarNodo(raiz, nuevo);
    fixInsercion(raiz, nuevo);
  }

  void mostrarInorden() { inorden(raiz); }
  int totalNodos() { return contarNodos(raiz); }

  void consultaPorRangoDeFechas(const string &fechaInicio,
                                const string &fechaFin,
                                int limiteMuestra = 20) {
    int totalEncontradas = 0;
    cout << "\n  --- Muestra de resultados (max. " << limiteMuestra
         << ") ---\n";
    consulRanFech(raiz, fechaInicio, fechaFin, totalEncontradas, limiteMuestra);
    if (totalEncontradas == 0) {
      cout << "  [!] No se encontraron transacciones en este rango.\n";
    } else {
      if (totalEncontradas > limiteMuestra) {
        cout << "  ... (se omitieron " << (totalEncontradas - limiteMuestra)
             << " resultados adicionales)\n";
      }
      cout << "\n============================================\n";
      cout << " TOTAL ENCONTRADAS: " << totalEncontradas << " transacciones\n";
      cout << "============================================\n";
    }
  }

  // Consulta ordenada por fecha y hora: muestra solo las primeras 'limite'
  // transacciones (20 por defecto) usando el recorrido inorden del arbol.
  void consultarPorFechaHora(int limite = 20) {
    cout << "\n--- Transacciones ordenadas por fecha y hora (primeras "
         << limite << " de " << totalNodos() << ") ---\n";
    if (raiz == nullptr) {
      cout << "  [VACIO] No hay transacciones registradas.\n";
      return;
    }
    int mostrados = 0;
    inordenLimitado(raiz, limite, mostrados);
    cout << "----------------------------------------------------------\n";
  }

  // Eliminar por id del arbol
  bool eliminar(const string &id) {
    RBNode *nodo = buscarNodo(raiz, id);
    if (nodo == nullptr)
      return false;
    eliminarNodo(raiz, id);
    return true;
  }
};

int cargarCSV(const string &nombreArchivo, hashTable &hash,
              RedBlackTree &arbol) {

  ifstream archivo(nombreArchivo);
  if (!archivo.is_open()) {
    cout << "[ERROR] No se pudo abrir: " << nombreArchivo;
    return 0;
  }

  string linea;
  int cargadas = 0, errores = 0;

  while (getline(archivo, linea)) {
    if (linea.empty() || linea.substr(0, 2) == "id" ||
        linea.substr(0, 2) == "ID")
      continue;

    stringstream ss(linea);
    string campos[8];
    int i = 0;
    while (i < 8 && getline(ss, campos[i], ',')) {
      i++;
    }
    if (i < 8) {
      errores++;
      continue;
    }

    double monto = 0.0;
    try {
      monto = stod(campos[4]);
    } catch (...) {
      errores++;
      continue;
    }

    Transaccion *t = new Transaccion(campos[0], campos[1], campos[2], campos[3],
                                     monto, campos[5], campos[6], campos[7]);
    hash.insert(t);
    arbol.insertar(t);
    cargadas++;
  }
  archivo.close();

  cout << "\n============================================\n"
       << " CARGA COMPLETADA\n"
       << "============================================\n"
       << " Archivo     : " << nombreArchivo << "\n"
       << " Insertadas  : " << cargadas << " transacciones\n"
       << " Con errores : " << errores << " lineas ignoradas\n"
       << " En Hash     : " << cargadas << " registros\n"
       << " En RBT      : " << arbol.totalNodos() << " nodos\n"
       << "============================================\n\n";

  return cargadas;
}

bool buscarPorID(hashTable &hash, const string &id) {
  cout << "\n--- Buscando ID: " << id << " ---\n";
  Transaccion *t = hash.buscar(id);
  if (t != nullptr) {
    cout << "[ENCONTRADA]\n";
    t->mostrar();
    return true;
  } else {
    cout << "[ERROR] No existe transaccion con ID '" << id << "'.\n";
    return false;
  }
  cout << "--------------------------------\n";
}

void registrarTransaccion(hashTable &hash, string id, string cuenta, string cli,
                          string tip, double mon, string fec, string hor,
                          string est) {
  if (buscarPorID(hash, id) == true) {
    cout << "[ERROR] La transaccion con ID '" << id << "' ya existe.\n";
    return;
  } else {
    Transaccion *t = new Transaccion(id, cuenta, cli, tip, mon, fec, hor, est);
    hash.insert(t);
    RedBlackTree().insertar(t);
    cout << "[SUCCESS!] Se registro la transaccion correctamente. \n";
    t->mostrar();
  }
}

void EliminarPorID(hashTable &hash, RedBlackTree &arbol, const string &id) {
  cout << "\n--- Eliminando " << id << " ---\n";

  if (hash.eliminar(id) && arbol.eliminar(id)) {
    cout << "[EXITO] Transaccion eliminada de ambas estructuras\n";
  } else {
    cout << "[ERROR] No se pudo eliminar\n";
  }

  cout << "\nVerificando que ya no existe:\n";
  buscarPorID(hash, id);

  cout << "Total de nodos en RBT despues: " << arbol.totalNodos() << "\n";
}

void consulPorRangoDeFechas(RedBlackTree &arbol, const string &fechaInicio,
                            const string &fechaFin, int limiteMuestra = 20) {
  cout << "\n--- Consultando transacciones entre " << fechaInicio << " y "
       << fechaFin << " ---\n";
  arbol.consultaPorRangoDeFechas(fechaInicio, fechaFin, limiteMuestra);
}

// Actualiza el estado de una transaccion buscandola por ID en la hash table.
// Estados validos y sugeridos por el profeee:) : Aprobada, Rechazada,
// Pendiente, Observada, Anulada. Como ambas estructuras (hash y RBT) comparten
// el mismo Transaccion*, el cambio se refleja automaticamente en ambas (grande
// el que lo hizo)
void actualizarEstado(hashTable &hash, const string &id,
                      const string &nuevoEstado) {
  cout << "\n--- Actualizacion de estado ---\n";

  // Validar que el estado sea uno de los permitidos
  const string estadosValidos[] = {"Aprobada", "Rechazada", "Pendiente",
                                   "Observada", "Anulada"};
  bool esValido = false;
  for (int i = 0; i < 5; i++) {
    if (nuevoEstado == estadosValidos[i]) {
      esValido = true;
      break;
    }
  }
  if (!esValido) {
    cout << "[ERROR] Estado '" << nuevoEstado << "' no es valido.\n"
         << "  Estados permitidos: Aprobada, Rechazada, Pendiente, Observada, "
            "Anulada\n";
    return;
  }

  // Buscar la transaccion por ID en la hash table
  Transaccion *t = hash.buscar(id);
  if (t == nullptr) {
    cout << "[ERROR] No existe transaccion con ID '" << id << "'.\n";
    return;
  }

  // Mostrar estado anterior
  string estadoAnterior = t->estado;
  cout << "  ID          : " << t->idTransaccion << "\n"
       << "  Cliente     : " << t->cliente << "\n"
       << "  Estado ant. : " << estadoAnterior << "\n";

  // Actualizar el estado
  t->estado = nuevoEstado;

  cout << "  Estado nuevo: " << t->estado << "\n"
       << "[EXITO] Estado actualizado correctamente.\n";
}

int main() {

  hashTable hash(30011); // primo cercano a 10,000
  RedBlackTree arbol;

  cout << "========================================\n"
       << " ESCENARIO 1: Carga masiva\n"
       << "========================================\n";
{
  auto inicio = high_resolution_clock::now();
  cargarCSV("transacciones_masivo.csv", hash, arbol);
  auto fin = high_resolution_clock::now();
  auto duracion = duration_cast<milliseconds>(fin - inicio);
  cout << "Tiempo: " << duracion.count() << " milisegundos\n";
}
  cout << "====================================================\n"
       << " ESCENARIO 2: Prueba de insercion individual\n"
       << "====================================================\n";
/*{  
      string nuevoID = "TX-030004";
      cout << "\n  Insertando transaccion individual (" << nuevoID << ") con 30000 registros existentes...\n";

      Transaccion* nueva = new Transaccion(nuevoID, "001-11000", "Valentina Mora", "Transferencia", 2344.56, "2026-10-21", "08:10:15", "Pendiente");

      // Medir insercion en Hash Table
      auto t1_hash = high_resolution_clock::now();
      hash.insert(nueva);
      auto t2_hash = high_resolution_clock::now();
      auto duracion_hash = duration_cast<nanoseconds>(t2_hash - t1_hash);

      // Medir insercion en Red-Black Tree
      auto t1_rbt = high_resolution_clock::now();
      arbol.insertar(nueva);
      auto t2_rbt = high_resolution_clock::now();
      auto duracion_rbt = duration_cast<nanoseconds>(t2_rbt - t1_rbt);

      cout << "  > Transaccion insertada correctamente\n"
           << "  > ID: " << nueva->idTransaccion
           << " | Cliente: " << nueva->cliente
           << " | $" << fixed << setprecision(2) << nueva->monto << "\n"
           << "  Tiempo de insercion en Hash: " << duracion_hash.count() << " nanosegundos\n"
           << "  Tiempo de insercion en RBT:  " << duracion_rbt.count() << " nanosegundos\n";

      // Verificar que se inserto correctamente
      Transaccion* verificar = hash.buscar(nuevoID);
      if (verificar) {
        cout << "  [OK] Verificacion: transaccion encontrada en Hash despues de insercion\n";
      }
      cout << "  Total nodos en RBT: " << arbol.totalNodos() << "\n\n";
}
*/
  cout << "========================================\n"
       << " ESCENARIO 3: Busqueda por ID\n"
       << "========================================\n";
/*
  // --- Caso 1: Buscar la PRIMERA transaccion ---
  {
  string idBuscar = "TX-000001";
  cout << "\n  [Caso 1] Buscar PRIMERA transaccion (" << idBuscar << "):\n";

  auto t1 = high_resolution_clock::now();
  Transaccion* encontrada = hash.buscar(idBuscar);
  auto t2 = high_resolution_clock::now();

  auto duracion = duration_cast<nanoseconds>(t2 - t1);
  if (encontrada) {
    cout << "  > ENCONTRADA: " << encontrada->cliente
         << " | " << encontrada->tipo
         << " | $" << fixed << setprecision(2) << encontrada->monto
         << " | " << encontrada->estado << "\n";
  } 
  else {
    cout << "  > NO ENCONTRADA\n";
  }
  cout << "  Tiempo de busqueda en Hash: " << duracion.count() << " nanosegundos\n";
  }
  // --- Caso 2: Buscar una transaccion INTERMEDIA ---
  {
  string idBuscar = "TX-015000";
  cout << "\n  [Caso 2] Buscar transaccion INTERMEDIA (" << idBuscar << "):\n";

  auto t1 = high_resolution_clock::now();
  Transaccion* encontrada = hash.buscar(idBuscar);
  auto t2 = high_resolution_clock::now();

  auto duracion = duration_cast<nanoseconds>(t2 - t1);

  if (encontrada) {
    cout << "  > ENCONTRADA: " << encontrada->cliente
         << " | " << encontrada->tipo
         << " | $" << fixed << setprecision(2) << encontrada->monto
         << " | " << encontrada->estado << "\n";
  } else {
    cout << "  > NO ENCONTRADA\n";
  }
  cout << "  Tiempo de busqueda en Hash: " << duracion.count() << " nanosegundos\n";
  }
  // --- Caso 3: Buscar la ULTIMA transaccion ---
  {
    string idBuscar = "TX-030000";
    cout << "\n  [Caso 3] Buscar ULTIMA transaccion (" << idBuscar << "):\n";

    auto t1 = high_resolution_clock::now();
    Transaccion* encontrada = hash.buscar(idBuscar);
    auto t2 = high_resolution_clock::now();

    auto duracion = duration_cast<nanoseconds>(t2 - t1);

    if (encontrada) {
      cout << "  > ENCONTRADA: " << encontrada->cliente
           << " | " << encontrada->tipo
           << " | $" << fixed << setprecision(2) << encontrada->monto
           << " | " << encontrada->estado << "\n";
    } else {
      cout << "  > NO ENCONTRADA\n";
    }
    cout << "  Tiempo de busqueda en Hash: " << duracion.count() << " nanosegundos\n";
  }
  // --- Caso 4: Buscar un ID INEXISTENTE ---
  {
    string idBuscar = "TX-999999";
    cout << "\n  [Caso 4] Buscar ID INEXISTENTE (" << idBuscar << "):\n";

    auto t1 = high_resolution_clock::now();
    Transaccion* encontrada = hash.buscar(idBuscar);
    auto t2 = high_resolution_clock::now();

    auto duracion = duration_cast<nanoseconds>(t2 - t1);

    if (encontrada) {
      cout << "  > ENCONTRADA: " << encontrada->cliente
           << " | " << encontrada->tipo
           << " | $" << fixed << setprecision(2) << encontrada->monto
           << " | " << encontrada->estado << "\n";
    } else {
      cout << "  > NO ENCONTRADA\n";
    }
    cout << "  Tiempo de busqueda en Hash: " << duracion.count() << " nanosegundos\n";
  cout << "\n";
  }
*/
  cout << "====================================================\n"
       << " ESCENARIO 4: Consulta ordenada por fecha y hora\n"
       << "====================================================\n";
/*{
  cout << "  Estructura utilizada: Red-Black Tree (recorrido inorden)\n";

  auto t1 = high_resolution_clock::now();
  arbol.consultarPorFechaHora(
      20); // Muestra las primeras 20 transacciones ordenadas
  auto t2 = high_resolution_clock::now();

  auto duracion_consulta = duration_cast<microseconds>(t2 - t1);
  cout << "  Tiempo de consulta ordenada (20 primeras): "
       << duracion_consulta.count() << " microsegundos\n";
  }*/
  cout << "========================================\n"
       << " ESCENARIO 5 : Consulta por rango de fechas\n"
       << "========================================\n";
/*{
       cout << "  Estructura utilizada: Red-Black Tree (recorrido inorden con filtro)\n";

  // --- Prueba A: Rango de 1 semana ---
  {
    cout << "\n  [Prueba A] Rango de 1 semana (1 al 7 de febrero 2026):\n";

    auto t1 = high_resolution_clock::now();
    consulPorRangoDeFechas(arbol, "2026-02-01 00:00:00", "2026-02-07 23:59:59");
    auto t2 = high_resolution_clock::now();

    auto duracion = duration_cast<microseconds>(t2 - t1);
    cout << "  Tiempo de consulta (1 semana): " << duracion.count() << " microsegundos\n";
  }

  // --- Prueba B: Rango de 1 mes ---
  {
    cout << "\n  [Prueba B] Rango de 1 mes (todo marzo 2026):\n";

    auto t1 = high_resolution_clock::now();
    consulPorRangoDeFechas(arbol, "2026-03-01 00:00:00", "2026-03-31 23:59:59");
    auto t2 = high_resolution_clock::now();

    auto duracion = duration_cast<microseconds>(t2 - t1);
    cout << "  Tiempo de consulta (1 mes): " << duracion.count() << " microsegundos\n";
  }

  // --- Prueba C: Rango sin resultados ---
  {
    cout << "\n  [Prueba C] Rango sin resultados (año futuro 2028):\n";

    auto t1 = high_resolution_clock::now();
    consulPorRangoDeFechas(arbol, "2028-01-01 00:00:00", "2028-01-31 23:59:59");
    auto t2 = high_resolution_clock::now();

    auto duracion = duration_cast<microseconds>(t2 - t1);
    cout << "  Tiempo de consulta (sin resultados): " << duracion.count() << " microsegundos\n";
  }
}
*/
  cout << "========================================================\n"
       << " ESCENARIO 6: Actualizacion de estado de transaccion\n"
       << "========================================================\n";
/*      auto t1 = high_resolution_clock::now();

      actualizarEstado(hash, "TX-000001", "Anulada");
      actualizarEstado(hash, "TX-000010", "Rechazada");
      actualizarEstado(hash, "TX-000025", "Observada");
      actualizarEstado(hash, "TX-000100", "Aprobada");
      actualizarEstado(hash, "TX-000500", "Pendiente");
      actualizarEstado(hash, "TX-001000", "Anulada");
      actualizarEstado(hash, "TX-005000", "Aprobada");
      actualizarEstado(hash, "TX-010000", "Rechazada");
      actualizarEstado(hash, "TX-020000", "Observada");
      actualizarEstado(hash, "TX-025000", "Aprobada");

      auto t2 = high_resolution_clock::now();
      auto duracion = duration_cast<microseconds>(t2 - t1);
      cout << "\n  Tiempo total de 10 actualizaciones: " << duracion.count() <<" microsegundos\n";
*/
  cout << "========================================\n"
       << " ESCENARIO 7: Eliminacion\n"
       << "========================================\n";
    {
      auto t1 = high_resolution_clock::now();

      EliminarPorID(hash, arbol, "TX-000010");
      EliminarPorID(hash, arbol, "TX-000020");
      EliminarPorID(hash, arbol, "TX-000030");
      EliminarPorID(hash, arbol, "TX-000040");
      EliminarPorID(hash, arbol, "TX-000050");
      EliminarPorID(hash, arbol, "TX-000060");
      EliminarPorID(hash, arbol, "TX-000070");
      EliminarPorID(hash, arbol, "TX-000080");
      EliminarPorID(hash, arbol, "TX-000090");
      EliminarPorID(hash, arbol, "TX-000100");

      auto t2 = high_resolution_clock::now();
      auto duracion = duration_cast<milliseconds>(t2 - t1);
      cout << "\n  Tiempo total de 10 eliminaciones: " << duracion.count() << " milisegundos\n";
    }

  cout << "========================================\n"
       << " ESCENARIO 8: Estadisticas generales\n"
       << "========================================\n";

  // Estadisticas stats = hash.calcularEstadisticas();
  // stats.mostrar();

  return 0;
}
