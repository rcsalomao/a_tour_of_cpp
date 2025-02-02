#include <cassert>
#include <concepts>
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

namespace capitulo_8 {
// assim como no caso de fuções, construtores e métodos, o mecanismo de
// sobrecarga também está presente para o caso de concepts.

template <std::forward_iterator Iter>
void advance(Iter p, int n) {
    print("advance para 'forward_iterator'");
    while (n--) {
        ++p;
    }
}
template <std::random_access_iterator Iter>
void advance(Iter p, int n) {
    print("advance para 'random_access_iterator'");
    while (n--) {
        ++p;
    }
}

struct Foo {
    std::vector<int> m_vec;
    Foo(std::initializer_list<int> list) : m_vec{list} {};
    int& operator[](int i) {
        if (!(0 <= i && i < (int)m_vec.size())) {
            throw std::out_of_range("Vector::operator[]");
        }
        return m_vec[i];
    };
    void foo(int i) { print(m_vec[i]); }
};

struct Bar {
    std::vector<int> m_vec;
    Bar(std::initializer_list<int> list) : m_vec{list} {};
    int& operator[](int i) {
        if (!(0 <= i && i < (int)m_vec.size())) {
            throw std::out_of_range("Vector::operator[]");
        }
        return m_vec[i];
    };
    void bar(int i) { print(m_vec[i]); }
};

template <typename T>
    requires requires(T t, int i) {
        t[i];
        t.foo(i);
    }
void foobar(T&& t, int i) {
    print("foobar para tipos Foo");
    t[i] = 42;
    t.foo(i);
}

template <typename T>
    requires requires(T t, int i) {
        t[i];
        t.bar(i);
    }
void foobar(T&& t, int i) {
    print("foobar para tipos Bar");
    t[i] = 24;
    t.bar(i);
}
void use_advance(std::vector<int>::iterator&& vip,
                 std::list<std::string>::iterator&& lsp) {
    advance(lsp, 3);  // especifica para 'forward_iterator'
    advance(vip, 3);  // especifica para 'random_access_iterator'
}

// template <typename T>
// concept EqualityComparable = requires(T a, T b) {
//     { a == b } -> std::same_as<bool>;
//     { a != b } -> std::same_as<bool>;
// };

// implementação similar à anterior,
// entretanto esta também serve para
// comparações não homogêneas
template <typename T, typename T2 = T>
concept EqualityComparable = requires(T a, T2 b) {
    { a == b } -> std::same_as<bool>;
    { a != b } -> std::same_as<bool>;
    { b == a } -> std::same_as<bool>;
    { b != a } -> std::same_as<bool>;
};

// concept para representaçao de tipos numéricos e
// operações aritméticas entre dois tipos quaisquer
template <typename T, typename U = T>
concept Number = requires(T x, U y) {
    x + y;
    x - y;
    x* y;
    x / y;
    x += y;
    x -= y;
    x *= y;
    x /= y;
    x = x;
    x = 0;
};
template <typename T, typename U = T>
concept Arithmetic = Number<T, U> && Number<U, T>;

// para um exemplo um pouco mais complicado de concept:
// template <typename S>
// concept Sequence = requires(S a) {
//     typename std::ranges::range_value_t<S>;
//     typename std::ranges::iterator_t<S>;
//     { a.begin() } -> std::same_as<std::ranges::iterator_t<S>>;
//     { a.end() } -> std::same_as<std::ranges::iterator_t<S>>;
//     requires std::ranges::input_range<std::ranges::iterator_t<S>>;
//     requires std::same_as<std::ranges::range_value_t<S>,
//     std::iter_value_t<S>>;
// };
// Neste caso em específico, para uma definição de Sequencia, é mais válido
// fazer uso de definições já expressas na biblioteca padrão:
template <typename S>
concept Sequence = std::ranges::input_range<S>;

// caso deseje-se criar uma interface para escoder complexidade, pode-se
// realizar um alias de estruturas padrão, tal como:
template <class S>
using ValueType = typename S::value_type;

// Neste caso, a função faz uso do método '<', enquanto que a interface
// requerida pelo 'concept' apenas verifica a existência do método '=='. tal
// qual uma função template, caso algum tipo instanciado não possua o operador
// para satisfazer '<', resultará em erro de compilação apenas no momento de
// instanciação da função.
template <EqualityComparable T>
bool cmp(T a, T b) {
    return a < b;
}

// concepts também servem para criar restrições de tipo quando utilizados em
// conjunto com 'auto'. é extremamente poderoso quando empregado no caso de
// argumentos automaticamente deduzidos.
auto twice(Arithmetic auto&& x) { return x + x; }
auto thrice(auto x) { return x + x + x; }
// adicionalmente, pode-se utilizar os 'concepts' para restringir o tipo da
// variável de retorno deduzida por 'auto', de tal forma que pode-se ter uma
// melhor documentação do código e capturar de forma mais eficiente erros que
// porventura possa ocorrer.
auto fct(auto x) { return x * 42; }
Number auto some_function(int x) {
    return fct(x);
}  // irá retornar algum erro caso a função 'fct' retorne algum valor que não
   // obedeça ao concept 'Number'

// como processo para construção de funções genéricas, pode-se partir de uma
// implementação específica:
double sum(const std::vector<int>& v) {
    double res = 0;
    for (auto& x : v) {
        res += x;
    }
    return res;
}
// logo após, procede-se à analise e levantamento de restrições, tais como:
// - porque apenas inteiros?
// - porque apenas vetores?
// - porque acumular em uma variável do tipo 'double'?
// - porque começar com o valor inicial de '0'?
// - porque realizar a operação de soma?
// uma possível implementação que generaliza os pontos anteriormente levantados:
template <std::forward_iterator Iter, Arithmetic<std::iter_value_t<Iter>> Val>
Val accumulate(Iter first, Iter last, Val res) {
    for (auto p = first; p != last; ++p) {
        res += *p;
    }
    return res;
}

// templates variádicos, em resumo, são templates com número variável de
// argumentos paramétricos. segue um exemplo de função que printa no terminal n
// argumentos
template <typename T>
concept Printable = requires(T t) { std::cout << t; };

// void print() {};                           // para o caso de nanhum argumento
// template <Printable T, Printable... Tail>  // 'Printable...' indica que
//                                            // 'Tail' é uma sequência de tipos
// void print(T head,
//            Tail... tail) {  // 'Tail...' indica que que tail é uma sequência
//                             // de valores de tipos representados por 'Tail'.
//     std::cout << head << "";
//     print(tail...);  // um parâmetro declarado por '...' é chamado de pacote
//                      // de parâmetros. no caso, 'tail' é um pacote de
//                      // argumentos // cusjos elementos são dos tipos
//                      // encontrados no pacote de parâmetros 'Tail'. desta
//                      // forma, 'print' pode receber qualquer número de
//                      // argumentos de quaisquer tipos que possam ser
//                      // representados no terminal.
// }
//
// como é uma função recursiva, faz-se necessário a implementação do caso final,
// onde 'print' recebe nenhum argumento (print()) para interromper o processo.
//
// Caso deseje-se suprimir o caso final (print()), uma implementação alternativa
// para print pode ser dada como:
// template <Printable T, Printable... Tail>
// void print(T head, Tail... tail) {
//     std::cout << head << " ";
//     if constexpr (sizeof...(tail) > 0) {
//         print(tail...);
//     }
// }

// fold expressions:
template <Number... T>
Number auto sum(T... v) {
    // return (v + ... + 0);  // adiciona 'n' elementos, com o valor inicial de
    //                        // 0. esta configuração também é chamada de 'right
    //                        // fold': (v[0] + (v[1] + (v[2] + (v[n] + 0))))
    return (0 + ... + v);  // esta configuração é a 'left fold':
                           // ((((0 + v[0]) + v[1]) + v[2]) + v[n])
}

// ainda, é possível fazer uma nova implementação de print para tipos
// paramétricos variádicos utilizando as fold expressions:
template <Printable... T>
void print(T&&... args) {
    (std::cout << ... << args) << "\n";
}

// perfect forwarding arguments:
template <typename T>
concept InputTransport = requires {
    true;
};  // alguma definição de interface do que constitui os argumentos
    // InputTransport. depende do domínio do problema.
template <typename Transport, InputTransport... TransportArgs>
class InputChannel {
   public:
    // ...
    InputChannel(TransportArgs&&... transport_args)
        : _transport{std::forward<TransportArgs>(transport_args)...} {
          };  // neste ponto, os argumentos de entrada para construção do objeto
              // 'Transport' são repassados por 'perfect forwarding' para o
              // respectivo construtor.
    // ...
    Transport _transport;
};

void main() {
    use_advance(std::vector<int>{1, 2, 3, 4}.begin(),
                std::list<std::string>{"a", "b", "c", "d"}.begin());
    foobar(Foo{1, 2, 3, 4}, 2);
    foobar(Bar{0, 0, 0, 0}, 2);
    static_assert(
        EqualityComparable<int>);  // int implementa o 'concept' definido por
                                   // 'EqualityComparable'
    // static_assert(EqualityComparable<Foo>); // entretato, Foo causa erro
    // static_assert(
    //     EqualityComparable<int, std::string>);  // assim com este assert
    static_assert(EqualityComparable<int, double>);
    static_assert(EqualityComparable<std::string, const char*>);

    bool b0 = cmp(2, 3);
    // bool b1 = cmp(2, "hello");  // erro: não consegue encontrar uma função
    //                             // 'cmp' que satisfaça a condição, de
    //                             // acordo com os argumentos informados.

    auto x1 = twice(4);
    print(x1);
    // auto x2 = twice(
    //     std::string{"Hello"});  // erro: std::string não é um tipo numérico
    auto x3 = thrice(std::string{"Hello "});
    print(x3);
    // adicionalmente, concepts pode ser utilizados para restringir o tipo
    // deduzido por 'auto' no momento de inicialização das variáveis.
    Arithmetic auto x4 = twice(2.8);
    print(x4);
    print(some_function(3));

    std::vector<int> vi{1, 2, 3, 4, 5};
    print(accumulate(std::begin(vi), std::end(vi), 0.0));
    print("hello", "world", 42, 24, 35.9);

    print(sum(1, 2, 3, 4));
    print(sum(1.2, 2.4, 4.2, 3.9));
    print(sum('a', 2.4, 4.2, 3.9));
    print(3, " ", 2.4, " ", "foobar", " ", std::string{"world"});
};
}  // namespace capitulo_8
