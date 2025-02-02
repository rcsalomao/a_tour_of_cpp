#include <algorithm>
#include <any>
#include <cassert>
#include <chrono>
#include <concepts>
#include <filesystem>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <ostream>
#include <print>
#include <ranges>
#include <regex>
#include <set>
#include <span>
#include <spanstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <syncstream>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

template <typename T>
concept Printable = requires(T t) { std::cout << t; };
template <Printable... T>
void print(T&&... args) {
    (std::cout << ... << args) << "\n";
}

void print_iterable(auto&& iterable) {
    for (auto& x : iterable) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

namespace capitulo_15 {
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using namespace std::ranges;
using std::shared_ptr;
using std::unique_ptr;
using namespace std::literals::string_literals;

struct Entry {
    string name;
    int number;

    bool operator==(const Entry& e) const {
        return (name == e.name) && (number == e.number);
    }

    bool operator<(const Entry& y) const { return name < y.name; }
};

enum class ErrorCode { range_error, length_error, found, not_found };
std::unordered_map<ErrorCode, string> ErrorCodeMap = {
    {ErrorCode::range_error, "range_error"},
    {ErrorCode::length_error, "length_error"},
    {ErrorCode::found, "found"},
    {ErrorCode::not_found, "not_found"},
};

// Pointer
// de forma geral, um ponteiro nos permite acessa e manipular objetos, de acordo
// com seu tipo. para citar alguns tipos de ponteiros possíveis:
// T*: ponteiro 'builtin' que aponta para um objeto do tipo 'T' ou para uma
//     sequência alocada de forma contínua na memória de elementos do tipo 'T'
// T&: um tipo 'referência' para um objeto do tipo 'T'. trata-se de um ponteiro
//     com deferência implícita.
// unique_ptr<T>: um ponteiro que possui a posse do objeto de tipo 'T'.
//                quando destruído, automaticamente chama o destructor do objeto
//                possuído para limpar o recurso.
// shared_ptr<T>: um ponteiro com posse compartilhada para um objeto de tipo 'T'
//                somente quando o último ponteiro for destruído é que o
//                destructor do objeto possuído será chamado para limpar o
//                recurso.
// weak_ptr<T>: um ponteiro para um objeto possuído por um ponteiro 'shared_ptr'
// span<T>: um ponteiro para uma sequência contígua de objetos de tipo 'T'
// string_view<T>: ponteiro para uma 'const sub-string'
// {X}_iterator<C>: uma sequência de elementos de um container 'C'.
//                  'X' indica o tipo de iterador.

// uso de ponteiros permite que se passa trabalhar de forma bastatne eficiente,
// sem incorrer em cópias ou demais operações custosas. entretanto, deve-se ter
// atenção e cuidado para não incorrer em problemas associados ao mal
// gerenciamento dos recursos em memória. como forma de se minimizar a
// ocorrência desses acidentes, pode-se adotar certas medidas:
// - Não guardar ou retornar um ponteiro que aponta para um objeto de escopo
// local. evita-se de gerar um 'dangling pointer' no momento que ocorrer a
// destruição do objeto local.
// - usar quando possível os 'smart_pointers', pois estes são capazes de
// realizar o gerenciamento automático do recurso (realiza o processo de deletar
// e limpar o recurso quando estes ponteiros saem de escopo).
// - não há possibilidade de gerar 'dangling pointers' para objetos staticos
// (por exemplo, variáveis globais).
// - 'string_view' e 'span' são ponteiros do tipo 'non-owning', ou seja, eles
// não tem a posse do objeto que estão apontando.
//
// ainda, com o uso dos 'smart_pointers' é possível retornar um ponteiro de uma
// função:
template <typename X, typename... Args>
unique_ptr<X> factory_X_1(Args&&... args) {
    return std::unique_ptr<X>{new X{std::forward<Args>(args)...}};
}

// outra característica é que 'unique_ptr' são repassados por meio do mecanismo
// de 'move', enquanto 'shared_ptr' é repassado pelo mecanismo de 'copy'.
// como 'shared_ptr' irá destruir o objeto apenas quando o último ponteiro for
// destruído, este acaba se comportando como uma espécie de 'garbage collector'.
// entretanto, este comportamento dificulta avaliar o verdadeiro ciclo de vida e
// permanẽncia do objeto, pois várias cópias e vários processos podem acabar
// possuindo sua própria cópia interna.
//
// ainda, 'shared_ptrs', por ainda se tratarem de ponteiros (mas com uma
// contagem interna de quantos ponteiros ainda existem para realização de
// limpeza de recurso) não impede, nem possui mecanismos para controle de 'data
// races' e leitura/escrita no objeto.
//
// mas que fique claro, não Há a necessidade que se retornar um pointeira
// apontado para uma coleção de objetos à partir de uma função. um container que
// também serve de gerenciador do recurso também possui a capacidade de
// simplesmente e eficientemente realizar essas operações, seja por 'copy
// ellision' ou 'move semantics'.
//
// as funções 'make_unique' e make_shared' facilitam o processo de criação de um
// 'smart_pointer', além de também garantir que a criação, e por consequẽncia o
// gerenciamento do recurso, se dê de forma correta.
template <typename X, typename... Args>
unique_ptr<X> factory_X_2(Args&&... args) {
    return std::make_unique<X>(std::forward<Args>(args)...);
}

struct S {
    int i;
    string s;
    double d;
    //...
};

// Span
// 'span' serve o caso de se precisar trabalhar com estruturas de dados
// contiguas (arrays ou vetores por exemplo) por meio de ponteiros. um 'span' é
// então basicamente um par de um ponteiro mais o comprimento do objeto
// ({p, p->size()}). desta forma reduz-se significativamente as chances de
// inserção de bugs e erros no código pelo uso direto e incorreto dos valores
// referentes aos ponteiro e comprimento do objeto trabalhado. adicionalmente,
// um 'span' não possui a posse do objeto apontado e portanto não possui a
// responsabilidade de gerenciar o recurso em memória.

void fpn(int* p, int n) {
    for (int i = 0; i < n; i++) {
        p[i] = 0;
    }
}

void fs(std::span<int> p) {
    for (int& x : p) {
        x = 0;
    }
}

void f1(std::span<int> p) {}
void f2(std::span<int> p) {
    // ...
    f1(p);  // para os casos comuns em que 'spans' são repassados entre funções,
            // como argumentos de saída e entrada, tem-se uma interface mais
            // segura, simples e direta.
}

void f3(int* p, int sz) {};

std::pair<int, ErrorCode> complex_search(vector<Entry>& v, const string& s) {
    int number;
    ErrorCode err;
    if (auto p = std::ranges::find_if(v, [&s](auto& o) { return o.name == s; });
        p != v.end()) {
        number = p->number;
        err = ErrorCode::found;
    } else {
        number = 0;
        err = ErrorCode::not_found;
    }
    return {number, err};
};

// para iteração de elementos
template <size_t N = 0, typename... Ts>
constexpr void print(std::tuple<Ts...> tup) {
    if constexpr (N < sizeof...(Ts)) {
        cout << get<N>(tup) << "";
        print<N + 1>(tup);
    }
}

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;  // 'deduction guide'

void print_variant_1(string& s) { cout << s << endl; };
void print_variant_1(ErrorCode& e) { cout << ErrorCodeMap[e] << endl; };

void main() {
    auto p1 = factory_X_2<S>(1, "Ankh Morpork", 4.65);
    auto p2 = factory_X_2<S>(2, "Oz", 7.62);
    cout << std::format("{}; {}", p1->s, p2->s) << endl;

    int a[100];
    size_t x{50};
    fpn(a, 100);  // ok
    // fpn(a, 1000); // erro: tentativa de acesso de recurso fora da range
    // fpn(a + 10, 100); // erro: acesso de recurso fora da range
    fpn(a, x);

    fs(a);  // implicitamente cria 'span<int>{a, 100}'
    // fs(a,1000); // erro: argumento de entrada esperado é do tipo 'span'
    // fs({a, 1000});  // erro: 'range error'
    // fs({a + 10, 100}); // erro: 'range error'
    fs({a, x});
    // em casos comuns, a criação de um 'span' diretamente à partir de uma
    // 'array' traz maior segurança, pois o compilador já calcula em tempo de
    // compilação o tamanho do container. nos outros casos, a interface
    // explícita força o desenvolvedor a criar o 'span' explicitamente,
    // minimizando chances de erros sutis.
    //
    // Containers
    //
    // Array:
    // trata-se de uma sequência de elementos de tamanho fixo para um dado tipo,
    // em que seu número de elementos é dado em tempo de compilação. desta
    // forma, uma array pode ser alocada na stack, num objeto ou de forma
    // estatica. é melhor entendida como uma array embutida, mas com seu tamanho
    // já prédescrito, sem a possibilidade de conversão implícita para ponteiros
    // de outros tipos. não há overhead (em tempo de espaço) em comparação com
    // arrays embutidas. uma array contém os seus elementos de forma direta.
    // desta forma, uma array deve ser inicializada no momento de sua definição
    // por uma 'initializer list':
    int n1{2};
    const int n2{2};
    std::array<int, 3> a1 = {1, 23, 3};
    std::array<int, 3> a2 = {24, 42};
    // std::array<int> a3{1, 2, 3};  // erro: falta definir o tamanho da array
    // std::array<string, n1> a4{
    //     "John's",
    //     "Queens"};  // erro: argumentos precisam ser uma expressão constante
    std::array<string, n2> a5{"John's", "Queens"};
    // std::array<2> a6{"John's",
    //                  "Queens"};  // erro: falta definir o tipo dos objetos
    print_iterable(a1);
    print_iterable(a2);

    // f3(a1, a1.size());         // erro: falha na conversão dos tipos
    f3(a1.data(), a1.size());  // erro: falha na conversão dos tipos
    auto p =
        std::ranges::find(a1, 24);  // compatível com a interface de 'ranges'
    // um dos motivos para se utilizar 'array' ao invés de 'vector' estão no
    // fato de que uma array e seus elementos são alocadados diretamente na
    // stack, ao invés da heap (que é o caso do 'vector') e portanto tem-se aí
    // possibilidade de ganho em performance. algumas áreas de aplicação proíbem
    // o uso da heap (controle seguro em tempo real de sistemas críticos). o uso
    // de 'delete' leva à fragmentação ou exaustão de memória. em comparação com
    // uma array embutida, a std::array sabe o seu próprio tamanho e pode ser
    // copiada com operador '='.
    auto a7 = a5;  // cópia
    print_iterable(a7);
    std::array<string, 2> a8{"Sarah", "Connor"};
    a7 = a8;  // atribuição
    print_iterable(a7);
    // um bitset é uma sequência de 'N' bits, em que 'N' deve ser conhecido em
    // tempo de compilação. caso deseja-se dar nomes, ao invés de numerá-los,
    // melhor fazer uso de 'set' ou 'enums'.
    std::bitset<9> bs1{"110001111"};
    std::bitset<9> bs2{0b110001111};
    cout << bs1 << endl;
    cout << bs1.to_ulong() << endl;
    cout << bs2 << endl;
    auto bs3 = ~bs1;  // complemento de bs1
    cout << bs3 << endl;
    auto bs4 = bs1 & bs3;  // tudo zerado
    cout << bs4 << endl;
    auto bs5 = bs1 << 2;  // shift para a esquerda
    cout << bs5 << endl;
    auto binary = [](int i) {
        std::bitset<8 * sizeof(int)> b = i;  // assumindo 8-bit byte
        // cout << b.to_string() << endl;  // escrita da representação em string
        cout << b << endl;
    };
    binary(123);
    cout << bs1.all() << endl;
    cout << bs1.none() << endl;
    cout << bs1.any() << endl;
    cout << bs1.count() << endl;
    cout << bs1.flip(0).to_ulong() << endl;

    // Pair
    // em muitas situações, deseja-se retornar de uma função/método mais do que
    // apenas 1 valor. 'pair' é uma estrutura de dados que guarda 2 elementos,
    // que podem ser de tipos totalmente distintos. os membros de um 'pair' são
    // chamados de 'first' e 'second'. adicionalmente pode-se fazer uso de
    // 'structured binding' para realizar o desempacotamento dos membros em
    // variáveis próprias. se seus elementos também tiverem suas implementações,
    // um 'pair' provém os operadores '=', '==' e '<'. Por fim, dedução de tipos
    // permite a construção de 'pair' de forma expedida e simples.
    vector<Entry> phone_book = {
        {"David Hume", 123456},
        {"Karl Popper", 678910},
        {"Bertrand Arthur William Russel", 111213},
    };
    auto [entry_number, sucess] = complex_search(phone_book, "David Hume");
    if (sucess == ErrorCode::found) {
        cout << entry_number << " " << ErrorCodeMap[sucess] << endl;
    } else {
        cout << "Não foi possível encontrar essa entrada no registro." << endl;
    }

    // Tuple
    // tuplas são muito similares às 'pairs', só que podem contemplar número
    // maior de elementos que apenas 2. de resto, contempla basicamente as
    // mesmas operações que 'pair'.
    std::tuple t0{};  // tupla vazia
    std::tuple<string, int, double> t1{
        "Shark", 123, 3.1415};  // tipo da tupla especificado explícitamente
    auto t2 = t1;               // tipo da tupla deduzido pelo 'auto'
    std::tuple t3{"Herring"s, 10,
                  1.23};  // tipo da tupla deduzido automaticamente
    auto t4 = std::make_tuple("Girafa"s, 24,
                              2.4);  // tipo da tupla deduzido automaticamente
    // para realizar o acesso dos elementos de uma tupla, deve-se utilizar
    // 'get<n>()' com 'n' sendo a posição do elemento:
    cout << std::get<0>(t1) << " " << std::get<1>(t1) << " " << std::get<2>(t1)
         << endl;
    // se tudos os elementos forem de tipos distintos, tbm é possível acessá-los
    // por meio dos seus respectivos tipos:
    cout << std::get<string>(t1) << " " << std::get<int>(t1) << " "
         << std::get<double>(t1) << endl;
    // os valores contidos numa tupla, não precisam ser imutáveis e portanto é
    // possível alterar os respectivos valores.
    get<0>(t1) = "Taínha";  // escrita no elemento
    cout << std::get<string>(t1) << " " << std::get<int>(t1) << " "
         << std::get<double>(t1) << endl;
    // também é possível obter os elementos por 'structured binding'
    auto [fish, count, price] = t1;
    cout << fish << " " << count << " " << price << endl;
    fish = "Shark";  // 'structured binding' também permite escrita de valures
                     // nos elementos
    cout << fish << " " << count << " " << price << endl;
    // assim como com 'pair', 'tuple' oferece os operadores '=', '==', '<' se
    // seus elementos também implementam.
    print(std::tuple{"Gavin ", 4.2, " ", "Anya ", 9, "\n"});

    // Tipos Alternativos
    //
    // Variant
    // uma alternativa mais segura e conveniente do que 'union'. na tentativa de
    // acesso, retorna o valor ou um erro.
    using Variant1 = std::variant<string, ErrorCode>;
    Variant1 v1{"Jonny"};
    Variant1 v2{ErrorCode::not_found};
    auto process_variant = [](auto& o) {
        if (std::holds_alternative<string>(o)) {
            cout << std::get<string>(o) << endl;
        } else {
            auto& err = std::get<ErrorCode>(o);
            // ... processa o erro
            cout << ErrorCodeMap[err] << endl;
        }
    };
    process_variant(v1);
    process_variant(v2);
    // auto c = std::get<double>(v1);  // erro: 'double' não é um dos tipos que
    //                                 // podem ser armazenados dentro do
    //                                 // Variant1.
    // uma forma de implementar o padrão de visitor seria:
    std::visit(overloaded{
                   [](string& s) { cout << s << endl; },
                   [](ErrorCode& e) { cout << ErrorCodeMap[e] << endl; },
               },
               v1);
    std::visit(overloaded{
                   [](string& s) { cout << s << endl; },
                   [](ErrorCode& e) { cout << ErrorCodeMap[e] << endl; },
               },
               v2);
    // outra forma de implementação seria:
    std::visit([](auto& a) { print_variant_1(a); }, v1);
    std::visit([](auto& a) { print_variant_1(a); }, v2);

    // Optional
    // uma forma de se entender o tipo 'optional' é como uma forma especial de
    // 'variant<T*, >', ou como uma generalização da idéia de que se
    // trata de um T* ou de um '{}' (objeto vazio). um optional sempre retorna
    // um ponteiro.
    auto return_optional = [](int i) {
        std::optional<string> o;
        if (i < 42) {
            o = {};
        } else {
            o = "Olha aí";
        }
        return o;
    };
    if (auto a = return_optional(42)) {
        cout << *a << endl;  // 'optional' se comporta que nem um ponteiro.
                             // portanto deve ser deferenciado.
    } else {
        cout << "Valor menor que 42" << endl;
    }
    // entretanto a tentativa de deferenciar um 'optional' que não guarda nanhum
    // valor é de comportamento 'undefined'. uma excessão não é levantada e
    // portanto 'optional' não é um tipo seguro.
    // auto optional_function = [](std::optional<int> a) { return *a; };
    // optional_function({});  // sus: vai resultar numa deferência de um objeto
    //                         // vazio. comportamento 'undefined'.

    // Any
    // um objeto de tipo 'any' pode guardar qualquer valor arbitrário. trata-se
    // basicamente de um 'variant' sem restrições.
    std::any an = 3;
    int i1 = std::any_cast<int>(an);
    // int i1 = std::any_cast<double>(
    //     an);  // erro: ao tentar realizar um cast para um tipo não conforme,
    //           // levanta uma exceção do tipo 'std::bad_any_cast'.
    cout << i1 << endl;
};
}  // namespace capitulo_15
