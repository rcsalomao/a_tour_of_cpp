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
#include <span>
#include <spanstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <syncstream>
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

namespace capitulo_11 {
using std::cin;
using std::cout;
using std::endl;

void output_1(int&& i) { cout << "O valor de i é: " << i << endl; };

void output_2() {
    int b = 'b';   // 'char' é implicitamente convertido para 'int'
    char c = 'c';  // um caractere envolto em quotes simples é tratado como
                   // 'character constant'.
    cout << 'a' << b << c
         << endl;  // é possível realizar o encademaento das operações '>>'.
}

void input_1() {
    int i;
    double d;
    // cin >> i;
    // cin >> d;
    cin >> i >> d;  // tal qual em 'cout', também é possível realizar o
                    // encademaento de operações '>>' para o 'cin'.
}

void input_2() {
    cout << "Por favor, informe o seu nome:\n";
    std::string name;
    // cin >> name;  // incorreto: irá apenas pegar o primeiro argumento,
    //               // separado por espaços (' '). No caso de nomes,
    //               // irá guardar apenas o primeiro nome.
    getline(cin, name);
    cout << "Olá, " << name << endl;
}

// operações de i/o possuem estado e é possível fazer uso deste para testes
// lógicos e controle de fluxo.
std::vector<int> read_ints(std::istream& is, const std::string& terminator) {
    std::vector<int> res;
    for (int i;
         is >> i;) {  // a operação 'is >> i' retorna uma referência para 'is'.
                      // o seu teste retorna 'true' caso a stream esteja pronta
                      // para outra operação. retorna 'false' caso contrário.
        res.push_back(i);
    }

    if (is.eof()) {  // finaliza a execução e retorna o resultado caso seja
                     // atingido o fim do arquivo.
        return res;
    }
    if (is.fail()) {  // caso a conversão para 'int' resulte em fracasso,
                      // realizar a verificação se o valor é uma string com o
                      // valor dado por 'terminator'.
        is.clear();   // resetar o estado interno para 'good()'
        std::string s;
        if (is >> s && s == terminator) {
            return res;
        }
        is.setstate(std::ios_base::failbit);  // adiciona 'fail()' para o
                                              // estado de 'is'.
    }
    return res;
}

struct Entry {
    std::string name;
    int number;
};

// um operador de output ('<<') para uma ostream recebe uma ostream como
// primeiro argumento e quaiquer outro tipo a ser representado como segundo
// argumento.
std::ostream& operator<<(std::ostream& os, const Entry& e) {
    return os << "{\"" << e.name << "\"," << e.number << "}";
};

// um operador de input para o tipo 'Entry' é um pouco mais complicado pois é
// necessario realizar parsing e validação da informação vinda.
std::istream& operator>>(std::istream& is, Entry& e) {
    char c;
    char c2;
    if (is >> c && c == '{' && is >> c2 &&
        c == '"') {        // começa com '{', sequido de um '"'
        std::string name;  // começa o acumulador com uma string vazia ("")
        while (is.get(c) &&
               c != '"') {  // qualquer coisa antes de um '"' faz
                            // parte do nome 'name'. 'is>>c' dá skip em espaço
                            // branco por padrão, mas o método '.get(c)' não.
                            // desta forma o operador '>>' para o tipo 'Entry' é
                            // capaz de ignorar espaços em branco fora do nome,
                            // mas não dentro de nome.
            name += c;
        }
        if (is >> c && c == ',') {
            int number = 0;
            if (is >> number >> c &&
                c == '}') {          // realiza a leitura do número e de um '}'
                e = {name, number};  // atribuição dos valores na variável de
                                     // tipo 'Entry'
                return is;
            }
        }
    }

    is.setstate(std::ios_base::failbit);  // realiza registro de falha na stream
    return is;
}

void main() {
    output_1(3);
    output_2();
    // input_1();
    // input_2();
    // auto v = read_ints(cin, "bla");
    // cout << v[0] << v[1] << endl;
    std::ifstream file{"./src/capitulo_11/read_ints.txt"};
    auto v = read_ints(file, "bla");
    cout << v[0] << v[1] << v[2] << endl;
    cout << file.good() << endl;
    Entry e{"Fulano", 23};
    cout << e << endl;

    constexpr int i = 123;
    cout << i << "; " << std::hex << i << "; " << std::oct << i << "; "
         << std::dec << i << endl;
    constexpr double d = 123.556;
    cout << d << "; " << std::scientific << d << "; " << std::hexfloat << d
         << "; " << std::fixed << d << "; " << std::defaultfloat << d << endl;

    cout.precision(3);  // 'precision()' é um método 'global' que afeta todas as
                        // invocações de 'cout' após o mesmo ser settado.
    cout << "precision(3): " << d << endl;
    cout.precision(8);
    cout << "precision(8): " << d + 0.000000005 << endl;
    cout.precision(16);
    cout << "precision(16): " << d + 0.000000005 << endl;

    cout << "zt: "
         << std::chrono::zoned_time{std::chrono::current_zone(),
                                    std::chrono::system_clock::now()}
         << endl;

    cout << std::format("Hello, {}", "World") << endl;
    cout << std::format("{0}; {0:x}; {0:o}; {0:d}; {0:b}", i) << endl;
    cout << std::format("{3}; {0:d}; {1:x}; {2:b}", 000, 111, 222, 333) << endl;
    cout << std::format("{0}; {0:.3}; {0:.8}; {0:.16}", d + 0.000000005)
         << endl;

    using namespace std::chrono_literals;
    auto ymd = 2025y / std::chrono::March / 30;
    cout << ymd << endl;
    cout << std::format("ymd: {3:%A},{1:%B} {2:}, {0:}", ymd.year(),
                        ymd.month(), ymd.day(), std::chrono::weekday(ymd))
         << endl;

    // std::string s = std::format(
    //     "{:%F}", 2);  // erro: erro de compilação. ccls consegue capturar
    //     este erro de argumentos
    //                   // inválidos para formatação de string na função
    //                   // 'format'.

    std::string fmt_1 = "valor do vformat_1: {}";
    int j = 2;
    cout << std::vformat(fmt_1, std::make_format_args(j))
         << endl;  // vformat traz a funcionalidade da função 'format' para que
                   // se possa executar em tempo de execução.
    // std::string fmt_2 = "valor do vformat_2: {:%F}";
    // int k = 2;
    // cout << std::vformat(fmt_2, std::make_format_args(k))
    //      << endl;  // entretanto, erros que seriam capturados em tempo de
    //                // compilação, ou pelo lsp, somente aparecem em tempo de
    //                // execução.

    // o formatador pode ser ainda escrito diretamente num buffer:
    std::string buf;
    std::format_to(std::back_inserter(buf), "iterador: {} {}", "Olá", 2025);
    cout << buf << endl;

    // existem diversas classes de streams:
    //
    // Standard Streams:
    // cout, cerr (unbuffered error output), clog (buffered logging output),
    // cin.
    //
    // File Streams:
    // ifstream (leitura de arquivos), ofstream (escrita de arquivos), fstream
    // (leitura e escrita de arquivos)
    std::ofstream ofs{"./src/capitulo_11/target"};
    if (!ofs) {
        throw std::runtime_error(
            "Não foi possível abrir 'target' para realizar a escrita");
    }
    ofs << std::format("ymd: {3:%A},{1:%B} {2:}, {0:}", ymd.year(), ymd.month(),
                       ymd.day(), std::chrono::weekday(ymd))
        << endl;
    std::ifstream ifs{"./src/capitulo_11/target"};
    if (!ifs) {
        throw std::runtime_error(
            "Não foi possível abrir 'target' para realizar a leitura");
    }
    cout << ifs.rdbuf();
    // assumindo que as operações de instanciar 'ofs' e/ou 'ifs' sejam bem
    // sucedidas, pode-se utilizar essas streams da mesma maneira que 'cout'
    // e/ou 'cin' respectivamente.
    //
    // String Streams:
    // a biblioteca <sstream> provém:
    // 'istringstream' para leitura de uma string, 'ostringstream' para escrita
    // para uma string e 'stringstream' para leitura e escrita em um string.
    std::ostringstream oss;
    oss << "{temperatura, " << std::scientific << 123.4567890 << "}";
    cout << oss.view()
         << endl;  // o conteúdo de uma stringstream pode ser lido por meio dos
                   // métodos 'str()' (retorna uma cópia do tipo string) ou
                   // 'view()' (retorna uma 'string_view'). um possível uso de
                   // uma 'ostringstream' é realizar a formatação antes de
                   // passar a stream para uma GUI.
    //
    // Memory Streams:
    // tratam-se de streams atreladas a seções designadas na memória. as funções
    // ofertadas são 'ispanstream', 'ospanstream' e 'spanstream'. Se comportam e
    // são inicializadas de forma similar as 'stringstreams', com a exceção de
    // que se uma uma 'span' ao invés de uma string.
    std::array<char, 128> buff;
    std::ospanstream spanstream{buff};
    spanstream << "escrita de " << 42 << "na memória" << endl;
    //
    // Synchronized Streams:
    // num contexto de multi-threads, a 'osyncstream' é capaz de garantir que
    // uma sequência de operações de output sejam executadas e seu resultado
    // será o resultado esperado no buffer de saída, mesmo quando houver
    // diversas threads tentando realizar operações de escrita.
    std::osyncstream osync{cout};
    int x{42};
    std::string y{"quarenta e dois"};
    osync << x << endl;
    osync << y << endl;
    // outras threads que também fazem uso de 'osyncstream' não irão interferir.
    // entretanto, outra thread que fizer uso de 'cout' diretamente poderá ser
    // capaz de interferir, então deve-se fazer uso consistente de 'osyncstream'
    // ou se certificar de que apenas uma thread seja capaz de produzir output.

    // File System
    // as funcionalidades para que se possa operar com arquivos e o sistema de
    // arquivos do sistema operacional se encontram na biblioteca <filesytem>.
    std::filesystem::path f = "./src/capitulo_11/read_ints.txt";
    assert(std::filesystem::exists(f));
    if (std::filesystem::is_regular_file(f)) {
        cout << f.filename()
             << " é um arquivo de tamanho: " << std::filesystem::file_size(f)
             << endl;
    }
    // a validade de um path não é verificada até o momento de seu uso.
    // listagem de todos os arquivos dentro de determinada pasta:
    std::filesystem::path p = "./src/capitulo_11";
    try {
        if (std::filesystem::is_directory(p)) {
            cout << "-> " << p << ":\n";
            for (const std::filesystem::directory_entry& fl :
                 std::filesystem::directory_iterator{p}) {
                cout << "    -> " << fl.path() << "\n";
            }
        }
    } catch (const std::filesystem::filesystem_error& ex) {
        std::cerr << ex.what() << endl;
    }
    if (std::filesystem::is_directory(p)) {
        cout << p << ":\n";
        for (const std::filesystem::directory_entry& fl :
             std::filesystem::directory_iterator{p}) {
            const std::filesystem::path& f =
                fl;  // referência para a parte 'path' do diretório de entrada
            if (f.extension() == ".exe") {
                cout << "    " << f.stem()
                     << " é um executável para Windows.\n";
            } else {
                std::string n = f.extension().string();
                if (n == ".cpp" || n == ".C" || n == ".cxx") {
                    cout << "    " << f.stem()
                         << " é um arquivo fonte de C++.\n";
                }
                if (n == ".txt") {
                    cout << "    " << f.stem() << " é um arquivo de texto.\n";
                }
            }
        }
    }
    // a biblioteca <filesystem> oferece também funções para manipulações:
    // p, p1 e p2 são 'paths'. 'e' é um 'error_code' e 'b' é 'bool'.
    // exists(p)
    // exists(p, e)
    // copy(p1, p2)
    // copy(p1, p2, e)
    // b = copy_file(p1, p2)
    // b = create_directory(p)
    // b = create_directories(b)
    // p = current_path()
    // current_path(p)
    // s = file_size()
    // b = remove(p)
    //
    // Por fim, existem também funções para realizar verificações sobre os
    // arquivos a serem inspecionados.
    // f é um 'path', ou um 'file_status'
    // is_block_file(f)
    // is_character_file(f)
    // is_directory(f)
    // is_empty(f)
    // is_fifo(f)
    // is_other(f)
    // is_regular_file(f)
    // is_socket(f)
    // is_symlink(f)
    // status_known(f)
};
}  // namespace capitulo_11
