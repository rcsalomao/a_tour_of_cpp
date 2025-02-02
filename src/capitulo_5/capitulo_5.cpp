#include <cassert>
#include <format>
#include <iostream>
#include <list>
#include <numeric>
#include <print>
#include <ranges>
#include <stdexcept>
#include <unordered_map>
#include <variant>
#include <vector>

void print(auto&& v) { std::println("{}", v); }

namespace capitulo_5 {
class complex {
    double re;
    double im;

   public:
    complex() : re{0}, im{0} {};
    complex(double r)
        : re{r},
          im{0} {};  // construtor de um argumento define operação de conversão
    complex(double r, double i) : re{r}, im{i} {};

    complex(const complex& other)
        : re{other.re}, im{other.im} {};  // copy contructor

    double real() const { return re; }  // getter
    void real(double d) { re = d; }     // setter
    double imag() const { return im; }  // getter
    void imag(double d) { im = d; }     // setter

    complex& operator+=(const complex& z) {
        re += z.re;
        im += z.im;
        return *this;
    }
    inline complex& operator-=(const complex& z) {
        re -= z.re;
        im -= z.im;
        return *this;
    }
    // Por questão de performance, prefere-se idealmente que as funções sejam do
    // tipo 'inline'. Por padrão os métodos definidos dentro do escopo de classe
    // são definidos como 'inline'.

    // declarações de operadores, definidas em outro local,
    // fora do escopo da definição da classe.
    complex& operator*=(const complex&);
    complex& operator/=(const complex&);
};

inline complex& complex::operator*=(const complex& z) {
    re *= z.re;
    im *= z.im;
    return *this;
}
inline complex& complex::operator/=(const complex& z) {
    re /= z.re;
    im /= z.im;
    return *this;
}

// Estas definições, dado o fato de não precisarem acesso direto à representação
// do objeto 'complex', pode ser definidas separadamente da definição da classe.
// Pode-se entender como um overload dos operadores para operarem em cima do
// objeto 'complex'.
complex operator+(complex a, complex b) { return a += b; }
complex operator-(complex a, complex b) { return a -= b; }
complex operator*(complex a, complex b) { return a *= b; }
complex operator/(complex a, complex b) { return a /= b; }
complex operator-(complex a) { return {-a.real(), -a.imag()}; }
bool operator==(complex a, complex b) {
    return a.real() == b.real() && a.imag() == b.imag();
}
bool operator!=(complex a, complex b) { return !(a == b); }

class Vector {
   public:
    Vector() : elem{nullptr}, sz{0} {}
    Vector(int s, double valor = 0.0) {
        if (s < 0) {
            throw std::length_error{"Vector constructor: negative size"};
        }
        if (s == 0) {
            throw std::length_error{"Vector constructor: zero size"};
        }
        elem = new double[s];
        sz = s;
        for (int i = 0; i < s; i++) {
            elem[i] = valor;
        }
    };
    Vector(std::initializer_list<double>&& list)
        : elem{new double[list.size()]}, sz{static_cast<int>(list.size())} {
        std::ranges::copy(list, elem);
    }
    ~Vector() {
        delete[] elem;
    }  // destructor: liberação do recurso previamento alocado
    double& operator[](int i) {
        if (!(0 <= i && i < size())) {
            throw std::out_of_range("Vector::operator[]");
        }
        return elem[i];
    };
    int size() const { return sz; };
    void push_back(double d);

   private:
    double* elem;
    int sz;
};

class Container {  // classe abstrata que decalra a interface a ser definida
                   // pelas classes concretas
   public:
    virtual double& operator[](int) = 0;
    virtual int size() const = 0;
    virtual ~Container() {};
};

class VectorContainer
    : public Container {  // classe concreta, derivada da classe abstrata
                          // 'Container', que define a interface requisitada
   public:
    VectorContainer(int s) : v(s) {}
    ~VectorContainer() {}

    double& operator[](int i) override {
        return v[i];
    }  // a sintaxe 'override' é opcional, mas deixa claro em código a intenção
       // do método e permite que o compilador possa detectar determinados
       // erros, tais como de tipografia, definição, tipos, etc.
    int size() const override { return v.size(); }

   private:
    Vector v;
};

class ListContainer
    : public Container {  // Outros tipos de containers, desde que obedecendo a
                          // interface requisitada, podem ser definidos e
                          // utilizados sem problemas.
   public:
    ListContainer() {}
    ListContainer(std::initializer_list<double> list) : ld{list} {}
    ~ListContainer() {}

    double& operator[](int i) override {
        for (auto& x : ld) {
            if (i == 0) {
                return x;
            }
            --i;
        };
        throw std::out_of_range("ListContainer");
    }
    int size() const override { return ld.size(); }
    auto give_me_address() const { return std::addressof(ld); }

   private:
    std::list<double> ld;
};

void use(Container& c) {
    const int sz = c.size();
    for (int i = 0; i < sz; i++) {
        print(c[i]);
    }
}

void main() {
    complex z{1, 0};
    const complex cz{1, 3};
    z = cz;  // no caso do material de referência, este se baseia no mecanismo
             // de definição implícita do 'copy assignment', que está
             // depreciado. Deve-se, portanto, quando existir definição própria
             // do 'copy constructor', também suprir definição do 'copy
             // assignment'.
    // cz = z;  // erro: não é possível realizar atribuição em variável
    // constante
    double x = z.real();
    complex a{2.3};
    complex b{1 / a};
    complex c{a + z * complex{1, 2.3}};
    if (c != b) {
        c = -(b / a) + 2 * b;
    };

    Vector v{1, 2.4, 3, 4};
    print(v.size());
    print(v[1]);
    VectorContainer vc{3};
    use(vc);
    ListContainer lc{1, 24, 42, 3.33};
    use(lc);

    // Container* con = &lc;
    Container* con = &vc;
    if (VectorContainer* p = dynamic_cast<VectorContainer*>(con)) {
        print("Vector container");
    } else if (auto* p = dynamic_cast<ListContainer*>(con)) {
        std::cout << "List container with addres: " << p->give_me_address()
                  << std::endl;
    }
}
}  // namespace capitulo_5
