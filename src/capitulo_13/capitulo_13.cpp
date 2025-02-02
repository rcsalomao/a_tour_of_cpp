#include <algorithm>
#include <cassert>
#include <chrono>
#include <concepts>
#include <execution>
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

void print_iterable(auto&& iterable) {
    for (auto& x : iterable) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

namespace capitulo_13 {
using std::cerr;
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

    bool operator<(const Entry& y) const { return name < y.name; }
};

struct EntryHasher {
    size_t operator()(const Entry& e) const {
        return std::hash<string>()(e.name) ^ std::hash<int>()(e.number);
    }
};

// seria possível então, criar uma função que retorne um 'vector' com todos os
// iteradores/ponteiros dos elementos que fossem equivalentes à um determinado
// valor 'V':
template <typename Cont, typename V>
vector<typename Cont::iterator> get_all(Cont& c, V v) {
    std::vector<typename Cont::iterator> res;
    for (auto p = c.begin(); p != c.end(); p++) {
        if (*p == v) {
            res.push_back(p);
        }
    }
    return res;
};
// uma versão mais simplificada por meio de 'range_value_t' que retorna um vetor
// de ponteiros para os elementos que atendem ao critéiro:
template <typename T>
using range_value_t =
    T::value_type;  // obter o tipo dos elementos de um container.
template <typename Cont, typename V>
auto get_all_v2(Cont& c, V v) {
    std::vector<range_value_t<Cont>*> res;
    for (auto& x : c) {
        if (x == v) {
            res.push_back(&x);
        }
    }
    return res;
}

struct GreaterThan {  // functor
    int val;
    GreaterThan(int v) : val{v} {}
    bool operator()(const std::pair<string, int>& r) const {
        return r.second > val;
    }
};

void main() {
    std::vector<int> vi1{1, 3, 4, 2, 5};
    print_iterable(vi1);
    std::sort(vi1.begin(),
              vi1.end());  // os algoritmos providos pela 'stl' possuem uma api,
                           // de forma geral, similares, em função de iteradores
                           // providos pela estrutura de dados container. o
                           // iterador retornado por 'begin()' aponta para o
                           // primeiro elemento do container, enquanto que o
                           // iterador retornado por 'end()' aponta para a
                           // posição 1 unidade após o último elemento.
    print_iterable(vi1);
    vector<Entry> phone_book = {
        {"David Hume", 123456},
        {"Karl Popper", 678910},
        {"Bertrand Arthur William Russel", 111213},
    };
    for (auto& x : phone_book) {
        std::cout << x.name << "; ";
    }
    std::cout << std::endl;
    std::sort(
        phone_book.begin(),
        phone_book.end());  // ao implementar determinados operadores nos tipos
                            // próprios, é possível também fazer uso dos mesmos
                            // algoritmos da 'stl'. os operadores em específico
                            // dependerão do tipo de algoritmo a ser utilizado.
                            // para 'sort', deve-se implementar os operadores de
                            // '<' e '=='.
    for (auto& x : phone_book) {
        std::cout << x.name << "; ";
    }
    std::cout << std::endl;
    // caso, por exemplo, deseja-se realizar a cópia de elementos únicos de um
    // 'vector' para uma lista, uma implemntação possível seria:
    vector<Entry> phone_book2 = {
        {"David Hume", 123456},
        {"Karl Popper", 678910},
        {"Bertrand Arthur William Russel", 111213},
    };
    std::list<Entry> le;
    std::sort(phone_book2.begin(), phone_book2.end());
    std::unique_copy(phone_book2.begin(), phone_book2.end(),
                     std::back_inserter(le));
    // 'back_inserter' constrói um iterador para 'le' que adiciona um elemento
    // ao fim do container, extendendo o container para criar mais espaço para
    // acomodar o novo elemento. isso nos poupa de ter de alocar um nova
    // quantidade fixa de espaço para preenchimento. desta forma 'back_inserter'
    // elimina a necessidade de se gerenciar manualmente a memória e, por
    // consequência, reduz a chance de erros. 'list' possui um 'move
    // constructor', o que faz com que o retorno por valor de funções seja uma
    // operação de baixo custo, em comparação com operações de cópia.
    le.push_back({"Fulano de Tal", 141516});
    for (auto& x : le) {
        std::cout << x.name << "; ";
    }
    std::cout << std::endl;

    // Use of Iterators:
    // os algoritmos da 'stl' operam em função dos iteradores disponibilizados
    // pelos tipos containers a serem trabalhados. desta forma se promove um
    // desacoplamento entre os algoritmos e os tipos containers, tornando o
    // código extremamente flexível. novos tipos podem ser definidos para serem
    // operados pelos mesmo algoritmos da 'stl', caso estes também possuam
    // implementações de iteradores e operadores próprios específicos.
    //
    // por exemplo, para operações de busca:
    string b{"blablabla"};
    char c{'a'};
    if (auto p = std::find(b.begin(), b.end(), c); p != b.end()) {
        cout << std::format("foi possível encontrar o valor '{}' na posição {}",
                            c, p - b.begin())
             << endl;
    } else {
        cout << std::format("não foi possível encontrar o valor '{}'", c)
             << endl;
    }
    // caso deseja-se encontrar todas as ocorrências de determinado valor com o
    // uso de iteradores:
    std::vector<int> posições;
    for (auto p = b.begin(); p != b.end(); p++) {
        if (*p == c) {
            auto posição = p - b.begin();
            posições.push_back(posição);
        }
    }
    // for (size_t i = 0; i < b.size(); i++) {
    //     if (b[i] == c) {
    //         posições.push_back(i);
    //     }
    // }
    for (auto& x : posições) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
    for (auto p : get_all(b, 'l')) {
        cout << std::format("({}, {})", *p, p - b.begin()) << " ";
    }
    cout << endl;
    for (auto p : get_all_v2(b, 'b')) {
        cout << *p << " ";
    }
    cout << endl;
    // Stream Iterators:
    // a 'stl' também provém iteradores para 'input stream' e 'output stream':
    std::ostream_iterator<string> oo{
        cout};  // escreve valores do tipo 'std::string' em 'cout'.
    *oo = "Olá, ";
    *oo = "mundo!";
    *oo = "\n";

    std::vector<string> v{"Hello", ", ", "World", "\n"};
    std::copy(v.begin(), v.end(), oo);

    // similarmente para 'input stream' por meio do iterador 'istream_iterator'.
    // de forma geral, 'istream_iterator' e 'ostream_iterator' não são
    // utilizados diretamente, mas são utlizados como argumentos de entrada para
    // algoritmos.
    // ainda, pode-se utilizar esses iteradores no contexto de leitura e escrita
    // de arquivos:
    //
    // std::ifstream is{
    //     "./src/capitulo_13/input_file.txt"};  // input stream para o arquivo
    //                                           // de entrada
    // std::istream_iterator<string> ii{is};     // input iterator para a stream
    // std::istream_iterator<string>
    //     eos{};  // 'input sentinel'. o valor padrão para uma variável
    //             // 'istream_iterator' é 'eof()' (final do arquivo).
    //
    // std::ofstream os{
    //     "./src/capitulo_13/output_file.txt"};  // output stream para o
    //                                            // arquivo de saída.
    // std::ostream_iterator<string> ooi{os,
    //                                   "\n"};  // 'ouput iterator' para a
    //                                           // stream,
    //                                           // mais definição do separador.
    //
    // std::vector<string> bb{
    //     ii, eos};           // 'bb' é um vetor de strings inicializado pela
    //                         // 'istream_iterator' do arquivo de entrada.
    // std::ranges::sort(bb);  // ordenação do buffer.
    // std::ranges::unique_copy(bb,
    //                          ooi);  // cópia do buffer para o 'ouput stream',
    //                                 // discartando itens adjascentes
    //                                 // similares.
    // cout << (!is.eof() || !os) << endl;
    //
    // adicionalmente, pode-se reescrever este procedimento de forma mais
    // concisa como:
    std::ifstream is{"./src/capitulo_13/input_file.txt"};
    std::ofstream os{"./src/capitulo_13/output_file.txt"};
    std::istream_iterator<string> ii{is};
    std::istream_iterator<string> eos{};
    std::set<string> bb{ii, eos};
    std::ostream_iterator<string> ooi{os, "\n"};
    std::ranges::copy(bb, ooi);
    cout << (!is.eof() || !os) << endl;

    // Predicates
    // alguns algoritmos podem receber predicados, funções ou functors que
    // conduzem e alteram o comportamento do respectivo algoritmo.
    std::map<string, int> m{{"A", 1},  {"B", 42}, {"C", 99},
                            {"D", 24}, {"E", 18}, {"F", 222}};
    auto pf = std::ranges::find_if(m, GreaterThan{42});
    cout << std::format("({}, {})", (*pf).first, (*pf).second) << endl;
    // ao invés de um 'functor', pode-se fazer uso de funções lambda:
    auto pl =
        std::ranges::find_if(m, [](const auto& r) { return r.second > 42; });
    cout << std::format("({}, {})", (*pl).first, (*pl).second) << endl;

    // Overview:
    // alguns dos algoritmos disponibilizados pela biblioteca <algorithm>:
    //
    // f = for_each(b, e, f)
    // p = find(b, e, x)
    // p = find_if(b, e, f)
    // p = count(b, e, x)
    // p = count_if(b, e, f)
    // replace(b, e, v, v2)
    // replace_if(b, e, f, v2)
    // p = copy(b, e, out)
    // p = copy_if(b, e, out, f)
    // p = move(b, e, out)
    // p = unique_copy(b, e, out)
    // sort(b, e)
    // sort(b, e, f)
    // (p1, p2) = equal_range(b, e, v)
    // p = merge(b, e, b2, e2, out)
    // p = merge(b, e, b2, e2, out, f)
    //
    // para cada um dos algoritmos que aceitam uma range de [b:e), existe uma
    // implementação declarada em '<ranges>' que recebe uma range como parâmetro
    // de entrada (ao invés dos iteradores 'begin()' e 'end()'). alguns
    // algoritmos podem alterar os elementos de um container ('replace()' ou
    // 'sort()' por exemplo), mas nenhum deles realizam a inserção ou remoção de
    // elementos. Para realizar a inserção deve-se fazer uso do iterador
    // 'back_inserter' ou utilizar algum método direto do container como
    // 'push_back()' ou 'erase()'. funções lambda são especialmente
    // interessantes para passar como argumentos à esses algoritmos.
    std::vector<int> vi2{1, 2, 3, 4, 5, 6, 7, 8};
    std::ranges::for_each(vi2, [](int& x) { x = x * x; });
    print_iterable(vi2);
    std::for_each(vi2.begin() + 4, vi2.end(), [](int& x) { x = x * x; });
    print_iterable(vi2);

    // Parallel Algorithms
    // Por meio da biblioteca <execution>, é possível executar os algoritmos de
    // forma paralela por meio de 'policies':
    //
    // seq: execução sequencial.
    // par: execução em paralela, quando possível.
    // unseq: execução não sequencial (vetorizada), quando possível.
    // par_unseq: execução em paralelo e/ou não sequencial, quando possível.
    //
    // em que execução em paralelo corresponde à execução de um conjunto de
    // atividades por meio de múltiplos threads (frequentemente rodando em
    // diversos núcleos de um processador). e execução vetorizada diz respeito à
    // execução em uma única thread mas utilizando o mecanismo de vetorização
    // (SIMD).
    //
    // pegando por exemplo 'sort()':
    std::vector<int> vi3{1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::sort(vi3.begin(), vi3.end());  // execução sequencial
    std::sort(std::execution::unseq, vi3.begin(),
              vi3.end());  // execução vetorizada
    std::sort(std::execution::par, vi3.begin(),
              vi3.end());  // execução paralela
    std::sort(std::execution::par_unseq, vi3.begin(),
              vi3.end());  // execução paralela e/ou vetorizada
    // essas políticas de execução apenas dicas, pois o compilador possui a
    // liberdade de executálas da forma que for melhor identificada.
};
}  // namespace capitulo_13
