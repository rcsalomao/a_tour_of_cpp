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

void print(auto&& v, std::string s = "{}") { std::println(s, v); }

namespace capitulo_6 {
class SomeType {};
class X {
   public:
    // um construtor que aceita apenas um argumento define uma conversão entre
    // tipos (X x = (SomeType)var;). Para evitar este mecanismo, pode-se
    // declarar o construtor como 'explicit'.
    // explicit X(SomeType);  // construtor em geral
    X(SomeType);  // construtor em geral
    X();          // construtor padrão
    // X(const X&) = delete;
    // copy constructor. Também é possível declarar por
    // meio de 'delete' que determinado construtor está
    // desativado e portanto não poderá ser utilizado.
    X(const X&);             // copy constructor
    X& operator=(const X&);  // copy assignment constructor
    X(X&&);                  // move constructor
    X& operator=(X&&);       // move assignment construtor
    ~X();                    // destructor
};

class complex {
    // é possível também definir inicializadores padrão para os membros de uma
    // classe no momento da definição.
    double re{0.0};
    double im{0.0};

   public:
    complex() {};
    complex(double r)
        : re{r}, im{0.0} {
          };  // construtor de um argumento define operação de conversão
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
    Vector(int s,
           double valor = 0.0) {  // construtor: responsável pela alocação dos
                                  // rescusos (new double[]).
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
    // deve-se implementar copy constructors e copy assignment constructors em
    // específico quando a classe é responsável por gerenciar recursos externos
    // (ponteiros, objetos alocados na heap, etc.)
    Vector(const Vector& other) : elem{new double[other.sz]}, sz{other.sz} {
        for (int i = 0; i < sz; i++) {
            elem[i] = other.elem[i];
        }
    }
    Vector& operator=(const Vector& other) {
        double* p = new double[other.sz];
        for (int i = 0; i < other.sz; i++) {
            p[i] = other.elem[i];
        }
        // aloca-se o recurso a ser copiado antes de se realizar a operação de
        // remoção (delete) para caso ocorra algum problema ou alguma exceção
        // seja levantada, o Vector original continua ainda retendo o recurso
        // original.
        delete[] elem;  // deletar o recurso alocado previamente para evitar
                        // vazamento de memória.
        // por fim, realiza-se a atualização dos recursos internos.
        elem = p;
        sz = other.sz;
        return *this;
    }
    // Para evitar processos custosos de cópia em operações com vetores, deve-se
    // também realizar a implementação de 'move constructor' e 'move
    // assignment'.
    Vector(Vector&& other)
        : elem{other.elem},
          sz{other.sz} {  // elem{other.elem} faz com que o ponteiro de Vector
                          // aponte para o mesmo recurso que do 'other'.
        other.elem =
            nullptr;   // deve-se então, para completar a operação de 'move',
                       // tornar nulo o ponteiro do vetor 'other' original.
        other.sz = 0;  // idem para o tamanho 'sz';
    };
    Vector& operator=(Vector&& other) {
        elem = other.elem;
        sz = other.sz;
        other.elem = nullptr;
        other.sz = 0;
        return *this;
    };
    ~Vector() {
        delete[] elem;
    }  // destructor: liberação do recurso previamento alocado
    double& operator[](int i) {
        if (!(0 <= i && i < size())) {
            throw std::out_of_range("Vector::operator[]");
        }
        return elem[i];
    };

    // Vários dos operadores comuns (+, -, >, &&, [], etc) podem ser
    // sobrescritos para tender os tipos próprios definidos (itens 6.4 - 6.5).

    // para os operadores de comparação, pode-se fazer a implementação dos
    // operadores de comparação 'three-way' e de equalidade. Para o operador de
    // comparação 'three-way', o valor 0 representa a equalidade, negativo
    // quando Vector for menor que 'other' e positivo quando Vector for maior
    // que 'other'.
    auto operator<=>(const Vector& other) const {
        return size() == other.size() ? 0 : size() < other.size() ? -1 : 1;
    }
    auto operator==(const Vector& other) const {
        return size() == other.size();
    }
    int size() const { return sz; };
    void push_back(double d);

    // operadores de containers involvem em boa parte 'iteradores'. Basicamente
    // todo container definido pela stl possui os métodos 'begin()' e 'end()',
    // com suas versões 'const' como 'cbegin()' e 'cend()'. Tais métodos
    // retornam iteradores, objetos essenciais para realizar a iteração ao longo
    // da estrutura de dados. Iteradores permitem que o container possa ser
    // utilizado em estruturas como 'range-for' e nos diversos algoritmos da
    // stl.

    // containers devem implementar '*', '->' e [] para acesso.
    // iteradores devem implementar '++', '--', '+=', '-=', '+', e '-'.
    // e operadores de cópia e 'move'.

    // Para alguns algoritmos e estruturas de dados, deve-se também fazer a
    // implementação dos métodos 'swap()' (ou implemente uma operação de 'move'
    // para fazer uso de 'std::swap') e 'hash<>'.

   private:
    double* elem;
    int sz;
};

void main() {};
}  // namespace capitulo_6
