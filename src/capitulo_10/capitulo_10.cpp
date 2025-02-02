#include <cassert>
#include <concepts>
#include <format>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <print>
#include <ranges>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>

template <typename T>
concept Printable = requires(T t) { std::cout << t; };
template <Printable... T>
void print(T&&... args) {
    (std::cout << ... << args) << "\n";
}

namespace capitulo_10 {
using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;
using std::regex;
using std::string;

// além de const char*, c++ oferece também objetos do tipo std::string que
// permitem operações e manipulações com variáveis de texto. também é oferecido
// o tipo std::string_view para manipulação de sequências de quaisquer tipo
// (seja string ou char[]). Adicionalmente também há a implementação de
// biblioteca de regex para trabalhar com padrões em texto.
//
// dentre as funções ofertadas, há a possibilidade de concatenação:
string compose(const string& name, const string& domain) {
    return name + "@" + domain;
}
// e adição 'inplace':
void m2(string& s1, string& s2) {
    s1 = s1 + "\n";
    s2 += "\n";
}

// objetos do tipo std::string também possuem 'move constructor', o que permite
// o retorno por valor de string de forma eficiente.

// também possui interface para operações de 'subscripting' ([]):
void m3() {
    string name = "Fulano de Tal";
    string s = name.substr(7, 12);  // "de Tal".
    print(s);
    name.replace(
        0, 6,
        "nicholas");  // o valor utilizado para a substituição não precisa ser
                      // do mesmo tamanho que o valor a ser substituído.
    name[0] = toupper(name[0]);
    print(name);
}

// operadores de comparação:
void respond(const string& answer) {
    string incantation{"abracadabra"};

    if (answer == incantation) {
        print("hocus pocus");
    } else if (answer == "yes") {
        print("nope");
    }
    // ...
}

// também é oferecido um objeto do tipo string_view, para trabalhar de forma
// universal com string, 'C-style' ou quaisquer outra representação de texto.
// uma string_view é basicamente um par de ponteiro e tamanho do texto
// representado ({pointer, size}). idéia semelhante à iteradores
string cat(std::string_view sv1, std::string_view sv2) {
    string res{sv1};  // inicialização de uma string por meio de uma string_view
    return res += sv2;
}
// como uma string_view é basicamente um par de ponteiro e tamanho, este por
// padrão não faz verificação 'out_of_bounds'. para tanto deve-se fazer uso do
// método 'at()' que levantará a exceção 'out_of_range', ou utilizar o objeto
// 'gsl::string_span'
std::string_view bad_string_view_use() {
    string s = "hello world";
    return {
        &s[0],
        sizeof(
            s)};  // problema: retornando um ponteiro para uma variável local,
                  // que será destruída ao fim da execução desta função.
}

// no caso do uso de regex e respectiva biblioteca, pode-se fazer uso de 'raw
// string literal', denotado por R"()" para representação da regex a ser
// compilada. uma 'raw string literal' é alimentada diretamente ao construtor e
// não há a necessidade de fazer o escape das barras '\' por '\\'.
// o suporte da biblioteca regex é:
// - std::regex_match(); -> casa uma regex contra uma string de tamanho
// conhecido.
// - std::regex_search(); -> procura por uma string que casa com uma
// regex de tamanho arbitrário numa stream de dados.
// - std::regex_replace(); -> procura por strings que casam com uma regex num
// stream de dados e as substitui.
// - std::regex_iterator(); -> iterador sobe 'matches' e 'submatches'.
// - std::regex_token_iterator(); -> iterador sobre 'não-matches'.

void use_regex_search() {
    std::ifstream in{"./src/capitulo_10/arquivo.txt"};
    if (!in) {
        std::cerr << "não há o arquivo arquivo.txt para abrir\n";
        return;
    }

    regex padrao{R"(\s\w+e\s(\w+\s)?)"};  // raw string literal ( R"()" )

    // formas de se ler o arquivo em sua totalidade:
    // forma 1:
    // string file_contents{std::istreambuf_iterator<char>(in),
    //                      std::istreambuf_iterator<char>()};
    // print(file_contents);
    // forma 2:
    // std::stringstream buffer;
    // buffer << in.rdbuf();
    // string file_contents = buffer.str();
    // print(file_contents);

    // leitura do arquivo uma linha por vez
    int line_no = 0;
    for (string line; getline(in, line);) {
        ++line_no;
        std::smatch matches;
        if (std::regex_search(line, matches, padrao)) {
            print("linha ", line_no, ":", matches[0]);
            if (1 < matches.size() && matches[1].matched) {
                print("\tsubmatch: ", matches[1]);
            }
        }
    }
}

void use_regex_match() {
    std::ifstream in{"./src/capitulo_10/arquivo.txt"};
    if (!in) {
        std::cerr << "não há o arquivo arquivo.txt para abrir\n";
        return;
    }

    regex padrao{R"(\w+e(?=\s|,))"};  // raw string literal ( R"()" ). Não
                                      // existe a sintaxe de 'look-behind'
                                      // ((?<=...)) na biblioteca regex da stl.
                                      // para casos mais complexos, talvez seja
                                      // melhor utilizar uma biblioteca externa

    // formas de se ler o arquivo em sua totalidade:
    // forma 1:
    // string file_contents{std::istreambuf_iterator<char>(in),
    //                      std::istreambuf_iterator<char>()};
    // print(file_contents);
    // forma 2:
    // std::stringstream buffer;
    // buffer << in.rdbuf();
    // string file_contents = buffer.str();
    // print(file_contents);

    // leitura do arquivo uma linha por vez
    int line_no = 0;
    for (string line; getline(in, line);) {
        ++line_no;
        string matches{""};
        for (std::sregex_iterator it =
                 std::sregex_iterator(line.begin(), line.end(), padrao);
             it != std::sregex_iterator(); it++) {
            std::smatch match{*it};
            matches += " ";
            matches += match[0];
        }
        print("linha ", line_no, ":", matches);
    }
}

void main() {
    auto addr = compose("fulano"s, "belelel-labs.com");
    print(addr);
    m3();
    respond("abracadabra");
    respond("yes");
    printf("Para pessoas que gostam de utilizar 'printf': %s\n",
           "Dog"s.c_str());  // string.c_str() retorna um ponteiro para os
                             // caracteres de string.
    std::cout << "Para pessoas que preferem usar streams: " << "Dog"s << "\n";
    string rei = "Harold";
    auto s1 = cat(rei, "William");
    auto s2 = cat(rei, rei);
    auto s3 = cat("Edward", "Stephen"sv);
    auto s4 = cat("Canute"sv, rei);
    auto s5 = cat({&rei[0], 2}, "Henry"sv);
    auto s6 = cat({&rei[0], 2}, {&rei[2], 4});
    print(s6);
    use_regex_search();
    use_regex_match();
};
}  // namespace capitulo_10
