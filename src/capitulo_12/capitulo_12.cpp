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

// namespace std {
// template <>
// struct hash<Entry> {
//     using argument_type = Entry;
//     using result_type = size_t;
//
//     result_type operator()(const Entry& e) const {
//         return hash<string>()(e.name) ^ hash<int>()(e.name);
//     }
// };
// }  // namespace std

namespace capitulo_12 {
using std::cout;
using std::endl;
using std::string;
using std::vector;

struct Entry {
    string name;
    int number;

    bool operator==(const Entry& e) const {
        return (name == e.name) && (number == e.number);
    }
};

struct EntryHasher {
    size_t operator()(const Entry& e) const {
        return std::hash<string>()(e.name) ^ std::hash<int>()(e.number);
    }
};

void main() {
    // Vector:
    //  por meio da inicialização universal, é possível criar uma estrutura de
    //  dados ('vector' neste caso) por meio de uma 'initializer_list' de
    //  'initializer_lists'.
    cout << "===== vector =====" << endl;
    vector<Entry> phone_book = {
        {"David Hume", 012345},
        {"Karl Popper", 678910},
        {"Bertrand Arthur William Russel", 111213},
    };
    // 'vector' possui operador '[]' e portanto é possível acessar e modificar
    // os elementos que estão no índice 'i':
    for (size_t i = 0; i < phone_book.size(); i++) {
        cout << phone_book[static_cast<int>(i)].name << endl;
    }
    // elementos de um 'vector' constituem uma 'range' e portanto podem ser
    // utilizados numa 'range-for':
    for (const auto& x : phone_book) {
        cout << x.name << endl;
    }
    // a inicialização por meio do operador '()' define para 1 argumento o
    // tamanho do 'vector', inicializado com o valor padrão para o seu
    // respectivo tipo. por outro lado, a inicialização com 2 argumentos (a, b)
    // gera um 'vector' de tamanho 'a' e com o valor padrão 'b'.
    vector<double> vd1{1, 2, 3, 4};
    vector<double> vd2(10);
    vector<double> vd3(99, 1.0);
    vector<Entry> ve(10);
    cout << ve[3].number << endl;
    // ainda, um 'vector' possui métodos tais como 'push_back' e 'emplace_back'
    // que, respectivamente, copiam elementos para o fim de um vetor e constroem
    // elementos no fim do vetor. adicionalmente, pode-se obter o tamanho de um
    // 'vector' por meio do método 'size()' e reservar à priori o tamanho deste
    // em memória por meio do método 'reserve'. o método 'clear' é capaz de
    // limpar o vetor, resultando na destruição de todos os objetos guardados e
    // resultando no tamanho final de 0.
    //
    // deve-se prestar atenção pois atribuições diretas entre 'vectors' são
    // 'copy assignment' e portanto geram a cópia dos elementos internos dos
    // vetores:
    vector<Entry> phone_book_2 = phone_book;                 // copy assignment
    vector<Entry> moved_phone_book = std::move(phone_book);  // move assignment
    // cout << phone_book[0].name
    //      << endl;  // erro: os objetos contidos em 'phone_book' foram movidos
    //                // para 'moved_phone_book'. tentar obter informação dostes
    //                // endereços de memória resulta em 'segfault'.
    cout << moved_phone_book[0].name << endl;
    // para o caso de se desejar um 'vector' para armazenar tipos com hierarquia
    // e posterior uso de polimorfismo (por meio de dispositivos virtuais).
    // recomenda-se fazer uso de ponteiros (mais específicamente, 'smart
    // pointers'):
    vector<std::unique_ptr<Entry>> vpe;
    vpe.emplace_back(std::make_unique<Entry>("David Hume", 012345));
    vpe.emplace_back(std::make_unique<Entry>("Karl Popper", 012345));
    vpe.emplace_back(
        std::make_unique<Entry>("Bertrand Arthur William Russel", 111213));
    cout << vpe[2]->name << endl;
    // a classe 'vector' não garante verificação de range por meio do seu
    // operador '[]'. caso seja desejável realizar esta verificação no momento
    // de acesso dos seus elementos, deve-se fazer uso do método 'at()', que irá
    // levantar uma excessão do tipo 'out_of_range' caso haja uma tentativa de
    // acessar um elemento fora da range do 'vector'.
    // cout << moved_phone_book[5].name
    //      << endl;  // muito ruim: não há verificação da range e
    //                // portanto não há garantias do seu comportamento.
    try {
        cout << moved_phone_book.at(5).name
             << endl;  // erro: método 'at()' irá levantar uma excessão do tipo
                       // 'out_of_range', que pode ser apanhada por uma cláusula
                       // try/catch ou caso contrário irá provocar o término da
                       // execução do programa de forma bem definida.
    } catch (std::out_of_range&) {
        std::cerr << "ocorreu um erro do tipo 'out_of_range' :3" << endl;
    } catch (...) {
        std::cerr << "tá, agora aconteceu algum outro erro não identificado :c"
                  << endl;
    }

    // List:
    // a implementação da lista da stl é de uma 'doubly-linked list'. logo, para
    // cada nó da lista, há um ponteiro para o próximo nó e para o nó anterior.
    cout << "===== list =====" << endl;
    std::list<Entry> lphone_book = {
        {"David Hume", 012345},
        {"Karl Popper", 678910},
        {"Bertrand Arthur William Russel", 111213},
    };
    // uma lista pode ser iterada de forma similar à um vetor:
    for (const auto& l : lphone_book) {
        if (l.name == "Karl Popper") {
            cout << l.number << endl;
        }
    }
    // uma lista também possui iteradores, por meio dos métodos 'begin()' e
    // 'end()'.
    for (auto p = lphone_book.begin(); p != lphone_book.end(); ++p) {
        if (p->name == "Karl Popper") {
            cout << p->number << endl;
        }
    }
    // a adição de elementos numa lista é feita pelo método 'insert(p, item)',
    // que insere o elemento 'item' (por meio de uma cópia) antes do elemento
    // apontado por 'p'.
    for (auto p = lphone_book.begin(); p != lphone_book.end(); ++p) {
        if (p->name == "Bertrand Arthur William Russel") {
            lphone_book.erase(p);
            break;  // sem esse, a iteração continuaria e resultaria em
                    // 'segfault'.
        }
    }
    for (auto p = lphone_book.begin(); p != lphone_book.end(); ++p) {
        if (p->name == "Karl Popper") {
            lphone_book.insert(p, Entry{"Fulano de Tal", 001122});
        }
    }
    for (const auto& l : lphone_book) {
        cout << l.name << endl;
    }

    // Forward_list:
    // é basicamente uma 'list' já apresentada, entretando permite apenas
    // iteração apenas para frente ('forward'). o ponto principal é economia de
    // espaço, já que uma 'Forward_list' vazia, por exemplo, ocupa o espaço de
    // um ponteiro.

    // Map:
    // a implementação do map em C++ e baseada na estrutura de árvore binária
    // balanceada 'red-black'.
    cout << "===== map =====" << endl;
    std::map<string, int> mphone_book = {
        {"David Hume", 12345},
        {"Karl Popper", 678910},
        {"Bertrand Arthur William Russel", 111213},
    };
    // o operador '[]' serve para obter um elemento por
    // meio de uma chave e se esta não for encontrada, o map cria
    // automáticamente uma nova entrada com o valor padrão de inicialização do
    // elemento correspondente.
    cout << mphone_book["David Hume"] << endl;
    cout << mphone_book["Karl Poper"] << endl;
    // 'map' também possui o método 'find()' que realiza a busca pela key e
    // evita de se realizar a inserção de novas entradas sem querer.
    mphone_book.erase("Karl Poper");
    if (std::map<string, int>::iterator a = mphone_book.find("Karl Poper");
        a != mphone_book.end()) {
        cout << a->second << endl;
    }

    // Unordered Map
    // enquanto 'map' faz uso de ordenamento ('<') para construir sua estrutura
    // interna, o que resulta numa árvore binária e permite operações de busca
    // com ordem de complexidade de O(log(n)), 'unordered_map' faz uso de uma
    // 'hash table' para construir sua estrutura de dados, o que resulta numa
    // ordem de busca com complexidade de O(1).
    cout << "===== unordered_map =====" << endl;
    std::unordered_map<string, int> hphone_book = {
        {"David Hume", 12345},
        {"Karl Popper", 678910},
        {"Bertrand Arthur William Russel", 111213},
    };
    cout << hphone_book["David Hume"] << endl;
    cout << hphone_book["david Hume"]
         << endl;  // de forma análoga à 'map', o uso do operador '[]' para
                   // chaves ainda não presentes na estrutura faz com que seja
                   // feita nova inserção de dado, com valores inicializados de
                   // forma padrão.
    hphone_book.erase("david Hume");
    if (std::unordered_map<string, int>::iterator a =
            hphone_book.find("Karl Popper");
        a != hphone_book.end()) {
        cout << a->second << endl;
    }
    cout << hphone_book.contains("David Hume")
         << endl;  // verifica se uma chave é presente na estrutura.
    cout << hphone_book.at("David Hume")
         << endl;  // caso a chave não seja encontrada, o método 'at()' levanta
                   // um exceção do tipo 'out_of_range'.
    // para a inserção de dados na estrutura, 'unordered_map' faz uso de função
    // 'hash' para o valor que servirá de chave ('key'). caso necessário é
    // possível suprir com função própria de 'hash' para operar sobre tipos
    // próprios. esta funcionalidade 'hash' para tipo próprio deve ser um
    // 'functor'. ainda, os tipos próprios devem também implementar o operador
    // '=='. em contraponto, uma estrutura do tipo 'map' deve implementar o
    // operador '<'.
    std::unordered_map<Entry, int, EntryHasher> h2phone_book = {
        {{"David Hume", 12345}, 0},
        {{"Karl Popper", 678910}, 1},
        {{"Bertrand Arthur William Russel", 111213}, 2},
    };
    // alternativamente, pode-se implementar uma especialização da função 'hash'
    // para o tipo próprio em específico, dentro do namespace 'std'.
    // a mesma api é presente para as estruturas de 'map', 'set' e
    // 'unordered_set'.

    // as estruturas de dados disponíveis pela stl, no processo de alocação,
    // usam por padrão 'new' e 'delete'. entretato, caso necessário, é possível
    // implementar e suprir um mecanismo própria para controlar o processo de
    // alocação e controle de memória dos elementos constituintes das estruturas
    // de dados.

    // Overview:
    // as estruturas de dados que a stl fornece:
    // 'bitset<N>'
    // 'array<T, N>'
    // 'vector<T>'
    // 'list<T>'
    // 'forward_list<T>'
    // 'queue<T>'
    // 'stack<T>'
    // 'deque<T>'
    // 'priority_queue<T>'
    // 'map<K, V>'
    // 'multimap<K, V>'
    // 'unordered_map<K, V>'
    // 'unordered_multimap<K, V>'
    // 'set<T>'
    // 'multiset<T>'
    // 'unordered_set<T>'
    // 'unordered_multiset<T>'

    // De forma geral, todos os tipos containers possuem uma api bem similar,
    // composta de forma geral pelos métodos:
    //
    // value_type -> tipo de um elemento
    // p = c.begin() -> p é um iterador e aponta para o primeiro elemento de
    // 'c'; 'cbegin()' é a versão 'const';
    // p = c.end() -> p é um iterador e aponta para um-além-do-último elemento
    // de 'c'; 'cend()' é a versão 'const';
    // k = c.size() -> k é o número de elementos em 'c';
    // c.empty() -> retorna booleano se 'c' está vazio ou não;
    // k = c.capacity() -> 'k' é a quantidade de elementos que 'c' pode
    // armazenar sem que haja nova alocação;
    // c.reserve(k) -> aumenta a capacidade para 'k' elementos; se k <=
    // c.capacity(), então não faz nada;
    // c.resize(k) -> Faz o número de elementos 'k'; elementos adicionados
    // possuem o valor padrão dado por 'value_type{}';
    //
    // c[k] -> acesso ao item de posição 'k'; Não há verificação de garantia de
    // que 'k' esteja dentro da range;
    // c.at(k) -> acesso ao item de posição 'k'; Se o valor 'k' estiver fora da
    // 'range' da estrutura, levanta uma exceção do tipo 'out_of_range';
    // c.push_back(x) -> adiciona o elemento 'x' ao fim de 'c'; incrementa o
    // tamanho 'c' em 1 unidade;
    // c.emplace_back(a) -> adiciona 'value_type{a}' ao fim de 'c'; incrementa o
    // tamanho 'c' em 1 unidade;
    // c.insert(p, x) -> adiciona o elemento 'x' antes de 'p' no container 'c';
    // c.erase(p) -> remove o elemento posicionado em 'p' do container 'c';
    //
    // c = c2 -> 'copy assignment' dos elementos de 'c2' para 'c' de tal forma
    // que 'c == c2';
    // b = (c == c2) -> equalidade de todos os elementos de 'c' e 'c2'; 'b ==
    // true' se forem iguais;
    // x = (c <=> c2) -> ordem lexicográfica de 'c' e 'c2'; 'x < 0' se 'c' for
    // menor que 'c2', 'x == 0' se 'c' e 'c2' forem iguais e ' x > 0' se 'c' for
    // maior que 'c2'; '!=', '<', '<=' '>' e '>=' são automaticamente gerados à
    // partir de '<=>';
};
}  // namespace capitulo_12
