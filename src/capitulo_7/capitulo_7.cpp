#include <cassert>
#include <format>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <print>
#include <ranges>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>

void print(auto&& v) { std::println("{}", v); }

namespace capitulo_7 {
template <typename T>
class Vector {
   public:
    Vector() : elem{nullptr}, sz{0} {}
    Vector(int s) {
        if (s < 0) {
            throw std::length_error{"Vector constructor: negative size"};
        }
        if (s == 0) {
            throw std::length_error{"Vector constructor: zero size"};
        }
        elem = new T[s];
        sz = s;
    };

    template <typename Iter>
    Vector(Iter, Iter){};  // Construtor para o caso de se construir à partir de
                           // dois iteradores
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        Iterator(T* p) : m_ptr{p} {};

        T& operator*() { return *m_ptr; };
        T* operator->() { return m_ptr; };
        Iterator& operator++() {
            m_ptr++;
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        friend bool operator==(const Iterator& a, const Iterator& b) {
            return a.m_ptr == b.m_ptr;
        };
        friend bool operator!=(const Iterator& a, const Iterator& b) {
            return a.m_ptr != b.m_ptr;
        };

       private:
        T* m_ptr;
    };

    Vector(int s, T valor) {
        if (s < 0) {
            throw std::length_error{"Vector constructor: negative size"};
        }
        if (s == 0) {
            throw std::length_error{"Vector constructor: zero size"};
        }
        elem = new T[s];
        sz = s;
        for (int i = 0; i < s; i++) {
            elem[i] = valor;
        }
    };
    Vector(std::initializer_list<T>&& list)
        : elem{new T[list.size()]}, sz{static_cast<int>(list.size())} {
        std::ranges::copy(list, elem);
    }
    Vector(const Vector<T>& other) : elem{new T[other.sz]}, sz{other.sz} {
        for (int i = 0; i < sz; i++) {
            elem[i] = other.elem[i];
        }
    }
    Vector& operator=(const Vector<T>& other) {
        double* p = new T[other.sz];
        for (int i = 0; i < other.sz; i++) {
            p[i] = other.elem[i];
        }
        delete[] elem;
        elem = p;
        sz = other.sz;
        return *this;
    }
    Vector(Vector<T>&& other) : elem{other.elem}, sz{other.sz} {
        other.elem = nullptr;
        other.sz = 0;
    };
    Vector& operator=(Vector<T>&& other) {
        elem = other.elem;
        sz = other.sz;
        other.elem = nullptr;
        other.sz = 0;
        return *this;
    };
    ~Vector() { delete[] elem; }
    T& operator[](int i) {  // para Vector não-const
        if (!(0 <= i && i < size())) {
            throw std::out_of_range("Vector::operator[]");
        }
        return elem[i];
    };
    const T& operator[](int i) const {  // para Vector const
        if (!(0 <= i && i < size())) {
            throw std::out_of_range("Vector::operator[]");
        }
        return elem[i];
    };
    auto operator<=>(const Vector<T>& other) const {
        return size() == other.size() ? 0 : size() < other.size() ? -1 : 1;
    }
    auto operator==(const Vector<T>& other) const {
        return size() == other.size();
    }
    int size() const { return sz; };
    // T* begin() const { return &elem[0]; };
    Iterator begin() const { return Iterator(&elem[0]); };
    // T* end() const { return &elem[0] + size(); };
    Iterator end() const { return Iterator(&elem[0] + size()); };
    void push_back(T d);

   private:
    T* elem;  // elem agora é um ponteiro para um array de tamanho 'sz' de tipo
              // 'T'
    int sz;
};

// declaração de um 'deduction guide' para ajudar o
// compilador na hora de realizar a instanciação do 'template'
// precisa ser feito fora do escopo de definição da classe
template <typename Iter>
Vector(Iter, Iter) -> Vector<typename Iter::value_type>;

void write(const Vector<std::string>& vs) {
    for (auto& v : vs) {
        std::cout << v << std::endl;
    }
}

// os argumentos a serem informados na definição 'template' também pode ser
// restringidos. Da-se o nome de 'concepts' para este mecanismo, de tal forma
// que um objeto ou função 'template' somente será instanciado se tais
// restrições forem válidas. Estas restrições servem para ajudar a garantir
// invariância do objeto ou função 'template'.

template <typename T,
          int N>  // definições de template também podem possuir argumentos de
                  // valor. Neste caso o argumento 'N' de tipo 'int' é um valor
                  // repassado para a construção do objeto 'Buffer'. Neste caso,
                  // permite-se construir um buffer sem fazer uso de alocação
                  // dinamica de memória (heap e uso de ponteiros).
struct Buffer {
    constexpr int size() { return N; }
    T elem[N];
    // ...
};

template <char* s>
void outs() {
    std::cout << s;
}

// função template parametrizada em dois tipos genéricos 'Sequence' e 'Value'.
template <typename Sequence, typename Value>
Value sum(const Sequence& seq, Value v) {
    for (auto x : seq) {
        v += x;
    }
    return v;
}

// definição de um 'function object' ou 'functor'
template <typename T>
class LessThan {
    const T val;

   public:
    LessThan(const T& v) : val{v} {};
    bool operator()(const T& x) const { return x < val; }
};

template <typename Container, typename Predicate>
int count(const Container& c, Predicate pred) {
    int count = 0;
    for (const auto& x : c) {
        if (pred(x)) {
            ++count;
        }
    }
    return count;
};

// também é possível fazer uso de lambdas para realizar operações de
// inicializaçao de objetos, segundo diversas condições
enum class InitMode { zero, value };
void use_vector(InitMode m, int n, int value) {
    // código de inicialização do vetor 'v' em função do modo de inicialização
    Vector<int> v = [&m, n, value] {
        switch (m) {
            case InitMode::zero:
                return Vector<int>(n, 0);
                break;
            case InitMode::value:
                return Vector<int>(n, value);
                break;
        }
    }();

    // operações com o vetor 'v'
    //...
}

// variable templates:
template <typename T, typename T2>
constexpr bool Assignable = std::is_assignable<T&, T2>::value;

template <typename T>
void test_assign() {
    static_assert(Assignable<T&, double>,
                  "can't assign a double to a type 'T'");
    static_assert(Assignable<T&, std::string>,
                  "can't assign a string to a type 'T'");
}

// é comum utilizar alias para questões de manutenabilidade, consistência e
// portabilidade de código.
template <typename T>
class Vetor {
   public:
    using value_type = T;
};

template <typename C>
using ValueType = C::value_type;

template <typename Container>
void algo(Container& c) {
    Vector<ValueType<Container>> vec;
    //...
};

template <typename key, typename Value>
class Map {
    // ...
};
template <typename Value>
using StringMap = Map<std::string, Value>;

// compile-time if:
// como a branch está como 'constexpr', esta é avaliada em tempo de compilação,
// estando para ser executado em tempo de execução a operação determinada pela
// condição 'std::is_trivially_copyable_v<T>'.
template <typename T>
void update(T& target) {
    // ...
    if constexpr (std::is_trivially_copyable_v<T>) {
        print("operação 1");
    } else {
        try {
            print("operação 2");
        } catch (...) {
            print("Ocorreu uma exceção!");
        }
    }
    // ...
}

void main() {
    Vector<std::string> v{"hello", "world"};
    write(v);
    Buffer<char, 1024> glob;
    Buffer<int, 10> int_buf;
    static char carr[] = "Olá mundo o/\n";
    outs<carr>();  // é possível repassar texto (no formato de char[]) como
                   // argumento para templates. o mesmo não é possível como
                   // literal ou std::string por motivos
    std::pair<int, double> par_1 = {1, 4.2};
    std::pair par_2 = {
        1, 4.2};  // em muitos contextos, é possível fazer uso de dedução
                  // automática dos tipos de um objeto/função template, por meio
                  // dos tipos dos argumentos do construtor.
    Vector v2{1, 2, 3, 4};         // automaticamente deduzido como Vector<int>
    Vector v3 = v2;                // idem
    auto p = new Vector{1, 2, 3};  // deduzido como Vector<int>*
    Vector<int> v4(
        2);  // neste caso é necessário informar o tipo para que se possa
             // realizar a construção do objeto template. Não é possível
             // realizar a dedução automática por meio deste construtor.
    // ainda assim, deve-se prestar atenção e usar esse mecanismo com cuidado:
    Vector<std::string> vs{"hello", "world"};  // ok
    Vector vs1{"hello", "world"};  // tipo deduzido: Vector<const char*>
    Vector vs2{
        std::string("hello"),
        std::string("world")};  // tipo deduzido: Vector<basic_string<char>>
    // Vector vs3{
    //     "hello",
    //     std::string("world")};  // erro: se os tipos dos argumentos do
    //                             // construtor forem de tipos diferentes,
    //                             // então o mecanismo de dedução não
    //                             // conseguirá realizar a dedução de tipos
    //                             // para o objeto template
    Vector vs4(4,
               2);  // sem o 'deduction guide', este construtor resultaria em
                    // erro por ser ambíguo. Com a definição do 'deduction
                    // guide', Vector é corretamente construído com o tipo 'int'
    // Vector vs5(v2.begin(),
    //            v2.end());  // por meio do deduction guide, Vector é
    //                        // corretamente  identificado como do tipo 'int'
    // Vector vs6{
    //     v2.begin(),
    //     v2.end()};  // ao empregar 'initializer_list', Vector passa a ser do
    //                 // tipo <Iterator> e acaba chamando o construtor errado.
    Vector v5{1.2, 1.3, 4.2, 2.4};
    print(sum(v2, 0));
    print(sum(v5, 0.0));

    LessThan lti{42};
    LessThan<std::string> lts{"Bigus"};
    print(lti(24));
    print(lts("Dicus"));
    print(count(v5, LessThan{2.0}));
    print(count(v2, LessThan{4}));
    // functor possuem a caracteristica interessante que são capazes de guardar
    // em si variáveis de estado. em alguns momentos, funções tais como
    // 'LessThan' também são chamadas de 'policy objects'.
    print(count(v5, [](const auto& v) { return v < 2.0; }));
    print(count(v2, [](const auto& v) { return v < 4; }));
    // também é possível definir objetos funções localmente no ponto de uso por
    // meio de lambdas. '[](const auto& v) { return v < 4; }' é uma expressão
    // lambda, enquanto que '[]' é chamada de 'capture list'. A lista de captura
    // pode capturar as variáveis que se encontram em escopo local adjacente ao
    // local de definicação da função lambda. '[&]' indica a captura de toda
    // variável por referência, enquanto que '[=]' indica a captura de toda
    // variável local por valor. também é possível capturar as variáveis por
    // nome como '[&x, y]' por exemplo. O modo de captura, quando não há
    // expecificador, é por valor.

    // test_assign<double>();  // erro: invoca a função template que acaba
    //                         // gerando erro num dos 'static_assert'.

    StringMap<int> m;
};
}  // namespace capitulo_7
