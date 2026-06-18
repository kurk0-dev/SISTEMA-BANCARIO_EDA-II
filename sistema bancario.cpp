#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

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

    Transaccion(string id, string cuenta, string cli, string tip,
                double mon, string fec, string hor, string est) {
        idTransaccion  = id;
        cuentaOrigen   = cuenta;
        cliente        = cli;
        tipo           = tip;
        monto          = mon;
        fecha          = fec;
        hora           = hor;
        estado         = est;
        fechaHoraOrden = fec + " " + hor;
    }

    void mostrar() const {
        cout << "  ID          : " << idTransaccion << "\n"
             << "  Cuenta      : " << cuentaOrigen  << "\n"
             << "  Cliente     : " << cliente       << "\n"
             << "  Tipo        : " << tipo          << "\n"
             << "  Monto       : " << monto         << "\n"
             << "  Fecha       : " << fecha         << "\n"
             << "  Hora        : " << hora          << "\n"
             << "  Estado      : " << estado        << "\n";
    }
};

class Node {
private:
    Transaccion* value;
    Node* next;
public:
    Node(Transaccion* t) { value = t; next = nullptr; }
    void setNext(Node* tmp) {next  = tmp;}
    Node* getNext() {return next;}
    Transaccion* getValue() {return value;}
};

class linkedList {
private:
    Node* head;
public:
    linkedList() { head = nullptr; }

    void insert(Transaccion* t) {
        Node* nodito = new Node(t);
        if (head == nullptr) { head = nodito; return; }
        Node* tmp = head;
        while (tmp->getNext() != nullptr) tmp = tmp->getNext();
        tmp->setNext(nodito);
    }

    // Busca por idTransaccion � recorre la cadena del bucket
    Transaccion* buscar(const string& id) {
        Node* tmp = head;
        while (tmp != nullptr) {
            if (tmp->getValue()->idTransaccion == id)
                return tmp->getValue();
            tmp = tmp->getNext();
        }
        return nullptr;
    }

    void print() {
        if (head == nullptr) { cout << "Vacio"; }
        else {
            Node* tmp = head;
            while (tmp != nullptr) {
                cout << tmp->getValue()->idTransaccion << " -> ";
                tmp = tmp->getNext();
            }
        }
        cout << "\n";
    }

    // Elimina un nodo por idTransaccion
    bool eliminar(const string& id) {
        if (head == nullptr) return false;

        // Si es el primer nodo
        if (head->getValue()->idTransaccion == id) {
            Node* tmp = head;
            head = head->getNext();
            delete tmp;
            return true;
        }

        // Recorre hasta encontrar el nodo anterior
        Node* actual = head;
        while (actual->getNext() != nullptr) {
            if (actual->getNext()->getValue()->idTransaccion == id) {
                Node* aEliminar = actual->getNext();
                actual->setNext(aEliminar->getNext());
                delete aEliminar;
                return true;
            }
            actual = actual->getNext();
        }
        return false; // No encontrado
    }

};

class hashTable {
private:
    linkedList* table;
    int size;

    // Hash para strings:
    int hashFunction(const string& key) {
        long long h = 0;
        for (int i = 0; i < (int)key.size(); i++)
            h = (h * 31 + (unsigned char)key[i]) % size;
        return (int)h;
    }

public:
    hashTable(int s) { size = s; table = new linkedList[size]; }
    ~hashTable()     { delete[] table; }

    void insert(Transaccion* t) {
        int pos = hashFunction(t->idTransaccion);
        table[pos].insert(t);
    }

    Transaccion* buscar(const string& id) {
        int pos = hashFunction(id);
        return table[pos].buscar(id);
    }

    void print() {
        for (int i = 0; i < size; i++) {
            cout << "Indice " << i << ": ";
            table[i].print();
        }
    }

    //Calcula el �ndice hash del ID y manda a realizar la eliminaci�n a la lista enlazada del bucket
    bool eliminar(const string& id) {
        int pos = hashFunction(id);
        return table[pos].eliminar(id);
    }
};

enum Color { ROJO, NEGRO };

class RBNode {
public:
    Transaccion* dato;
    Color  color;
    RBNode *izquierda, *derecha, *padre;

    RBNode(Transaccion* t) {
        dato      = t;
        color     = ROJO;
        izquierda = derecha = padre = nullptr;
    }
};

class RedBlackTree {
private:
    RBNode* raiz;

    void rotarIzquierda(RBNode*& raiz, RBNode*& x) {
        RBNode* y  = x->derecha;
        x->derecha = y->izquierda;
        if (y->izquierda != nullptr) y->izquierda->padre = x;
        y->padre = x->padre;
        if (x->padre == nullptr)            raiz = y;
        else if (x == x->padre->izquierda)  x->padre->izquierda = y;
        else                                x->padre->derecha   = y;
        y->izquierda = x;
        x->padre     = y;
    }

    void rotarDerecha(RBNode*& raiz, RBNode*& y) {
        RBNode* x    = y->izquierda;
        y->izquierda = x->derecha;
        if (x->derecha != nullptr) x->derecha->padre = y;
        x->padre = y->padre;
        if (y->padre == nullptr)            raiz = x;
        else if (y == y->padre->izquierda)  y->padre->izquierda = x;
        else                                y->padre->derecha   = x;
        x->derecha = y;
        y->padre   = x;
    }

    void fixInsercion(RBNode*& raiz, RBNode*& nodo) {
        RBNode* padre = nullptr, *abuelo = nullptr;
        while (nodo != raiz && nodo->color == ROJO && nodo->padre->color == ROJO) {
            padre  = nodo->padre;
            abuelo = padre->padre;
            if (padre == abuelo->izquierda) {
                RBNode* tio = abuelo->derecha;
                if (tio != nullptr && tio->color == ROJO) {
                    abuelo->color = ROJO; padre->color = NEGRO; tio->color = NEGRO;
                    nodo = abuelo;
                } else {
                    if (nodo == padre->derecha) {
                        rotarIzquierda(raiz, padre); nodo = padre; padre = nodo->padre;
                    }
                    rotarDerecha(raiz, abuelo);
                    swap(padre->color, abuelo->color);
                    nodo = padre;
                }
            } else {
                RBNode* tio = abuelo->izquierda;
                if (tio != nullptr && tio->color == ROJO) {
                    abuelo->color = ROJO; padre->color = NEGRO; tio->color = NEGRO;
                    nodo = abuelo;
                } else {
                    if (nodo == padre->izquierda) {
                        rotarDerecha(raiz, padre); nodo = padre; padre = nodo->padre;
                    }
                    rotarIzquierda(raiz, abuelo);
                    swap(padre->color, abuelo->color);
                    nodo = padre;
                }
            }
        }
        raiz->color = NEGRO;
    }

    void insertarNodo(RBNode*& raiz, RBNode*& nodo) {
        if (raiz == nullptr) { raiz = nodo; return; }
        if (nodo->dato->fechaHoraOrden < raiz->dato->fechaHoraOrden) {
            insertarNodo(raiz->izquierda, nodo);
            raiz->izquierda->padre = raiz;
        } else {
            insertarNodo(raiz->derecha, nodo);
            raiz->derecha->padre = raiz;
        }
    }

    void inorden(RBNode* nodo) {
        if (nodo == nullptr) return;
        inorden(nodo->izquierda);
        cout << nodo->dato->fechaHoraOrden
             << " | " << nodo->dato->idTransaccion
             << " | " << (nodo->color == ROJO ? "R" : "N") << "\n";
        inorden(nodo->derecha);
    }

    void inordenLimitado(RBNode* nodo, int limite, int& mostrados) {
        if (nodo == nullptr || mostrados >= limite) return;
        inordenLimitado(nodo->izquierda, limite, mostrados);
        if (mostrados >= limite) return;          // ya se completaron las N
        mostrados++;
        cout << "  [" << mostrados << "] "
             << nodo->dato->fechaHoraOrden
             << " | " << nodo->dato->idTransaccion
             << " | " << nodo->dato->cliente
             << " | " << nodo->dato->tipo
             << " | " << nodo->dato->monto
             << " | " << nodo->dato->estado << "\n";
        inordenLimitado(nodo->derecha, limite, mostrados);
    }

    //Consulta por rango de fecha "chef"
    void consulRanFech(RBNode* nodo, const string& fechaInicio, const string& fechaFin) {
        if (nodo == nullptr) return;
        if (nodo->dato->fechaHoraOrden >= fechaInicio) {
            consulRanFech(nodo->izquierda, fechaInicio, fechaFin);
        }
        if (nodo->dato->fechaHoraOrden >= fechaInicio && nodo->dato->fechaHoraOrden <= fechaFin) {
            cout << nodo->dato->fechaHoraOrden
                 << " | " << nodo->dato->idTransaccion
                 << " | " << (nodo->color == ROJO ? "R" : "N") << "\n";
        }
        if (nodo->dato->fechaHoraOrden <= fechaFin) {
            consulRanFech(nodo->derecha, fechaInicio, fechaFin);
        }
    }

    int contarNodos(RBNode* nodo) {
        if (nodo == nullptr) return 0;
        return 1 + contarNodos(nodo->izquierda) + contarNodos(nodo->derecha);
    }

    //Eliminaci n
    // Busca el nodo con el ID
    RBNode* buscarNodo(RBNode* nodo, const string& id) {
        if (nodo == nullptr) return nullptr;
        if (nodo->dato->idTransaccion == id) return nodo;

        RBNode* izq = buscarNodo(nodo->izquierda, id);
        if (izq != nullptr) return izq;
        return buscarNodo(nodo->derecha, id);
    }

    // Encuentra el sucesor inorden (nodo m nimo del sub rbol derecho)
    RBNode* minimo(RBNode* nodo) {
        while (nodo->izquierda != nullptr)
            nodo = nodo->izquierda;
        return nodo;
    }

    // Reemplaza un sub rbol
    void trasplantar(RBNode*& raiz, RBNode* u, RBNode* v) {
        if (u->padre == nullptr)
            raiz = v;
        else if (u == u->padre->izquierda)
            u->padre->izquierda = v;
        else
            u->padre->derecha = v;

        if (v != nullptr)
            v->padre = u->padre;
    }

    // Arregla violaciones de color despu�s de eliminar
    void fixEliminacion(RBNode*& raiz, RBNode* nodo) {
        while (nodo != raiz && color(nodo) == NEGRO) {
            if (nodo == nodo->padre->izquierda) {
                RBNode* hermano = nodo->padre->derecha;

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
                RBNode* hermano = nodo->padre->izquierda;

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
        if (nodo != nullptr) nodo->color = NEGRO;
    }

    // Funci�n auxiliar para obtener el color de un nodo (nullptr es NEGRO)
    Color color(RBNode* nodo) {
        return (nodo == nullptr) ? NEGRO : nodo->color;
    }

    // Elimina recursivamente
    void eliminarNodo(RBNode*& raiz, const string& id) {
        RBNode* nodo = buscarNodo(raiz, id);
        if (nodo == nullptr) return;

        RBNode* nodoAArreglar = nullptr;
        RBNode* padreAArreglar = nullptr;
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
            RBNode* sucesor = minimo(nodo->derecha);
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

        // Rebalancear si se elimin� un nodo negro
        if (colorOriginal == NEGRO && nodoAArreglar != nullptr) {
            fixEliminacion(raiz, nodoAArreglar);
        }
    }

public:
    RedBlackTree() { raiz = nullptr; }

    void insertar(Transaccion* t) {
        RBNode* nuevo = new RBNode(t);
        insertarNodo(raiz, nuevo);
        fixInsercion(raiz, nuevo);
    }

    void mostrarInorden() { inorden(raiz); }
    int  totalNodos()     { return contarNodos(raiz); }

    void consultaPorRangoDeFechas(const string& fechaInicio, const string& fechaFin) {
        consulRanFech(raiz, fechaInicio, fechaFin);
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

    //Eliminar por id del arbol
    bool eliminar(const string& id) {
        RBNode* nodo = buscarNodo(raiz, id);
        if (nodo == nullptr) return false;
        eliminarNodo(raiz, id);
        return true;
    }
};

int cargarCSV(const string& nombreArchivo,
              hashTable&    hash,
              RedBlackTree& arbol) {

    ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        cout << "[ERROR] No se pudo abrir: " << nombreArchivo;
        return 0;
    }

    string linea;
    int cargadas = 0, errores = 0;

    while (getline(archivo, linea)) {
        if (linea.empty() ||
            linea.substr(0, 2) == "id" ||
            linea.substr(0, 2) == "ID") continue;

        stringstream ss(linea);
        string campos[8];
        int i = 0;
        while (i < 8 && getline(ss, campos[i], ',')) {
            i++;
        }
        if (i < 8) { errores++; continue; }

        double monto = 0.0;
        try { monto = stod(campos[4]); }
        catch (...) { errores++; continue; }

        Transaccion* t = new Transaccion(
            campos[0], campos[1], campos[2], campos[3],
            monto, campos[5], campos[6], campos[7]
        );
        hash.insert(t);
        arbol.insertar(t);
        cargadas++;
    }
    archivo.close();

    cout << "\n============================================\n"
         << " CARGA COMPLETADA\n"
         << "============================================\n"
         << " Archivo     : " << nombreArchivo   << "\n"
         << " Insertadas  : " << cargadas        << " transacciones\n"
         << " Con errores : " << errores         << " lineas ignoradas\n"
         << " En Hash     : " << cargadas        << " registros\n"
         << " En RBT      : " << arbol.totalNodos() << " nodos\n"
         << "============================================\n\n";

    return cargadas;
}

bool buscarPorID(hashTable& hash, const string& id) {
    cout << "\n--- Buscando ID: " << id << " ---\n";
    Transaccion* t = hash.buscar(id);
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

void registrarTransaccion(hashTable& hash, string id, string cuenta, string cli, string tip,
                double mon, string fec, string hor, string est ){
    if(buscarPorID(hash, id) == true){
        cout << "[ERROR] La transaccion con ID '" << id << "' ya existe.\n";
        return;
    }
    else{
        Transaccion* t = new Transaccion(id, cuenta, cli, tip, mon, fec, hor, est);
        hash.insert(t);
        RedBlackTree().insertar(t);
        cout << "[SUCCESS!] Se registro la transaccion correctamente. \n";
        t->mostrar();
    }
}

void EliminarPorID(hashTable& hash, RedBlackTree& arbol, const string& id){
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

void consulPorRangoDeFechas(RedBlackTree& arbol, const string& fechaInicio, const string& fechaFin){
    cout << "\n--- Consultando transacciones entre " << fechaInicio << " y " << fechaFin << " ---\n";
    arbol.consultaPorRangoDeFechas(fechaInicio, fechaFin);
}

int main() {

    hashTable    hash(30011);   // primo cercano a 10,000
    RedBlackTree arbol;

    cout << "========================================\n"
         << " ESCENARIO 1: Carga masiva\n"
         << "========================================\n";
    cargarCSV("transacciones_masivo.csv", hash, arbol);

    cout << "========================================\n"
         << " ESCENARIO 2: Busqueda por ID\n"
         << "========================================\n";

    buscarPorID(hash, "TX-000001");   // primera transaccion
    buscarPorID(hash, "TX-005000");   // transaccion intermedia
    buscarPorID(hash, "TX-010000");   // ultima transaccion
    buscarPorID(hash, "TX-030000");   // ID inexistente

    cout << "====================================================\n"
         << " ESCENARIO 3: Consulta ordenada por fecha y hora\n"
         << "====================================================\n";
    arbol.consultarPorFechaHora(20);   // primeras 20 (usar 50 si se desea)

    cout << "====================================================\n"
         << " ESCENARIO 5: Prueba de insercion individual\n"
         << "====================================================\n";
    registrarTransaccion(hash, "TX-030001","001-11000", "Valentina Mora", "Transferencia", 2344.56,"2026-10-21","08:10:15","Pendiente");
    buscarPorID(hash, "TX-030001");

    cout << "========================================\n"
         << " ESCENARIO 6: (Falta Actualizacion) Eliminacion\n"
         << "========================================\n";

    EliminarPorID(hash, arbol, "TX-000001");//Eliminar primera transacci�n

    cout << "========================================\n"
         << " ESCENARIO 4 : Consulta por rango de fechas\n"
         << "========================================\n";
    consulPorRangoDeFechas(arbol, "2026-01-01 00:00:00", "2026-06-30 23:59:59");

    return 0;
}
