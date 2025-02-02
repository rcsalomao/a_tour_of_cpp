#include <cassert>
#include <format>
#include <numeric>
#include <print>
#include <stdexcept>
#include <unordered_map>
#include <variant>
#include <vector>

void print(auto&& v) { std::println("{}", v); }

namespace capitulo_4 {

enum class ErrorAction { ignore, throwing, terminating, logging };
constexpr ErrorAction default_error_action =
    ErrorAction::throwing;  // ação padrão de erro
enum class ErrorCode { range_error, length_error };
std::string error_code_name[]{"range error", "length error"};

// ao criar uma função centralizada para processamento de
// exceções, evita-se de espalhar a lógica de tratamento de
// exceção por todo o corpo de código do programa.
template <ErrorAction action = default_error_action, class C>
constexpr void expert(C cond, ErrorCode x) {
    if constexpr (action == ErrorAction::logging) {
        if (!cond()) {
            print(std::format("expect() failures: {} {}", int(x),
                              error_code_name[int(x)]));
        }
    }
    if constexpr (action == ErrorAction::throwing) {
        if (!cond()) {
            throw x;
        }
    }
    if constexpr (action == ErrorAction::terminating) {
        if (!cond()) {
            std::terminate();
        }
    }
}

class Vector {
   public:
    Vector(int s, double valor = 0.0) {
        if (s < 0) {
            throw std::length_error{"Vector constructor: negative size"};
        }
        if (s == 0) {
            throw std::length_error{"Vector constructor: zero size"};
        }
        // verificações iniciais no construtor, para que o objeto Vector possa
        // ser utilizado em sua plenitude. O comando 'throw' transfere o
        // controle para um handler para exceções do tipo 'out_of_range'.
        elem = new double[s];
        sz = s;
        for (int i = 0; i < s; i++) {
            elem[i] = valor;
        }
    };
    double& operator[](int i) {
        // if (!(0 < i && i < size())) {
        //     throw std::out_of_range("Vector::operator[]");
        // }
        // expert<ErrorAction::ignore>(
        //     [i, this] { return 0 < i && i < size(); },
        //     ErrorCode::range_error);  // ao utilizar ErrorAction::ignore,
        //                               // esta função fica efetivamente
        //                               // desabilitada, em tempo de
        //                               // compilação.
        expert<ErrorAction::logging>([i, this] { return 0 <= i && i < size(); },
                                     ErrorCode::range_error);
        return elem[i];
    };
    int size() { return sz; };

   private:
    double* elem;
    int sz;
};

void f(Vector& v) {
    try {
        v[3] += 41;  // uma operação qualquer com o vetor, que pode acabar
                     // resultando em alguma exceção.
    } catch (const std::out_of_range& err) {
        print(err.exception::what());
    }
}

void test(int n) {
    try {
        Vector v{n};
    } catch (std::length_error& err) {
        print("exceção: comprimento inválido");
        return;  // a cláusula de 'catch' por si não faz a interrupção do fluxo
                 // de execução.
    } catch (std::bad_alloc& err) {
        print("exceção: problema de alocação de memória");
        return;
    } catch (std::runtime_error& err) {
        print("exceção: error de execução");
        return;
    }
    print("função test: não ocorreu nenhuma exceção até aqui");
}

void f(const char* p) {
    assert(p !=
           nullptr);  // a macro assert é apenas compilada no mode de compilação
                      // em 'debug'. Do contrário, o 'assert' não é verificado.
    // ...
}

static_assert(
    4 <= sizeof(int),
    "valores inteiros são de tamanho muito pequenos.");  // verificação do
                                                         // tamanho dos valores
                                                         // inteiros da máquina.
constexpr double C = 299792.458;

void g(double speed) {
    constexpr double local_max =
        160.0 / (60 * 60);  // conversão de km/h para km/s
    // static_assert(
    //     speed < C,
    //     "Não dá para ir tão rápido quanto 'c'");  // erro: a variável 'speed'
    //                                               // deve ser uma constante
    static_assert(local_max < C, "Não dá para ir tão rápido quanto 'c'");
}

void user(
    int sz) noexcept {  // instrução 'noexcept' demarca uma função que não deve
                        // levantar exceções. Caso esta função levante uma
                        // exceção, a função std::terminate() é imediatamente
                        // invocada para finalizar o programa.
    Vector v{sz};
    std::iota(&v[0], &v[sz], 1.0);  // fill v com os valores 1,2,3,4,...
}

void main() {
    Vector v{4, 1.0};
    f(v);
    print(v[3]);
    test(-3);
    test(3);
    v[6];
}
}  // namespace capitulo_4
