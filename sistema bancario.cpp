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

    // Busca por idTransaccion — recorre la cadena del bucket
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

    int contarNodos(RBNode* nodo) {
        if (nodo == nullptr) return 0;
        return 1 + contarNodos(nodo->izquierda) + contarNodos(nodo->derecha);
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

void buscarPorID(hashTable& hash, const string& id) {
    cout << "\n--- Buscando ID: " << id << " ---\n";
    Transaccion* t = hash.buscar(id);
    if (t != nullptr) {
        cout << "[ENCONTRADA]\n";
        t->mostrar();
    } else {
        cout << "[ERROR] No existe transaccion con ID '" << id << "'.\n";
    }
    cout << "--------------------------------\n";
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

    return 0;
}
