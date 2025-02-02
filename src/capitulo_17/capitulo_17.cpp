#include <algorithm>
#include <any>
#include <bit>
#include <cassert>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <complex>
#include <concepts>
#include <execution>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <ostream>
#include <print>
#include <random>
#include <ranges>
#include <regex>
#include <set>
#include <source_location>
#include <span>
#include <spanstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <syncstream>
#include <thread>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <valarray>
#include <variant>
#include <vector>

namespace capitulo_17 {
using std::cerr;
using std::cout;
using std::endl;
using std::format;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;
using namespace std::ranges;
using namespace std::literals::string_literals;
using namespace std::literals::chrono_literals;
using namespace std::chrono;

template <typename T>
concept Printable = requires(T t) { std::cout << t; };
template <Printable... T>
void print(T&&... args) {
    (std::cout << ... << args) << std::endl;
}

template <typename T>
concept Iterable = requires(T t) {
    t.begin();
    t.end();
};
template <Iterable T>
void print(T&& iterable) {
    for (auto& x : iterable) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

void print(std::string&& s) { std::cout << s << std::endl; }
void print(std::string_view&& s) { std::cout << s << std::endl; }

// Numerics
//
// Algumas das funções providas pela biblioteca <numerics> e <cmath>:
// abs(x)
// ceil(x)
// floor(x)
// sqrt(x)
// cos(x)
// sin(x)
// tan(x)
// acos(x)
// asin(x)
// atan(x)
// sinh(x)
// consh(x)
// tanh(x)
// exp(x)
// exp2(x)
// log(x)
// log2(x)
// log10(x)
// as versões destas funções que operam em valores do tipo 'complex' são
// encontradas na biblioteca <complex>
//
// Os algoritmos providos por <numerics> são:
// x = accumulate(b, e, i)
// x = accumulate(b, e, i, f)
// x = inner_product(b, e, b2, i)
// x = inner_product(b, e, b2, i, f, f2)
// p = partial_sum(b, e, out)
// p = partial_sum(b, e, out, f)
// p = adjacent_difference(b, e, out)
// p = adjacent_difference(b, e, out, f)
// iota(b, e, v)
// x = gcd(n, m)
// x = lcm(n, m)
// x = midpoint(n, m)
//
// os algoritmos de execução paralela disponibilizados são:
// x = reduce(b, e, v)
// x = reduce(b, e)
// x = reduce(pol, b, e, v)
// x = reduce(pol, b, e)
// p = exclusive_scan(pol, b, e, out)
// p = inclusive_scan(pol, b, e, out)
// p = transform_reduce(pol, b, e, f, v)
// p = transform_exclusive_scan(pol, b, e, out, f, v)
// p = transform_inclusive_scan(pol, b, e, out, f, v)

class RandomInt {
   public:
    RandomInt(int low, int hight)
        : re{std::random_device{}()}, dist{low, hight} {}
    RandomInt(int low, int hight, unsigned long seed)
        : re{seed}, dist{low, hight} {}
    int operator()() { return dist(re); }
    void seed(int s) { re.seed(s); }

   private:
    std::default_random_engine re;
    std::uniform_int_distribution<int> dist;
};

template <typename T>
constexpr T tau_v = 2 * std::numbers::pi_v<T>;

constexpr double tau = tau_v<double>;

void main() {
    // erros são reportados settando 'errno', de <cerrno> para 'EDOM' para erro
    // de domínio e 'ERANGE' para erro de range:
    errno = 0;  // limpar o estado de erro anterior
    double d = std::sqrt(-1);
    if (errno == EDOM) {
        cerr << "sqrt() não definido para argumentos de valor negativo" << endl;
    }
    errno = 0;  // limpar o estado de erro anterior
    double dd = std::pow(std::numeric_limits<double>::max(), 2);
    if (errno == ERANGE) {
        cerr << "valor resultante de pow() muito grande para ser representado "
                "como um 'double'"
             << endl;
    } else {
        print("pow(double::max(), 2): ", dd);
    }

    // dos algoritmos encontrados em numerics:
    std::list<double> list{1, 2, 3, 45, 999.999};
    auto s = std::accumulate(list.begin(), list.end(), 0.24);
    print(s);
    // tais algoritmos funcionam para qualquer sequencia da 'stl' e podem ter
    // operações informadas como argumentos.
    //
    // <numerics> também provém algoritmos que podem ser executados de forma
    // paralela:
    std::vector<double> vd{1,  2,   34,  5,  5,  999.99, 42, 23,  2,  566, 76,
                           34, 656, 534, 52, 54, 634,    63, 452, 78, 68,  5};
    auto s2 = std::reduce(vd.begin(), vd.end());
    print(s2);
    auto s3 = std::reduce(std::execution::par, vd.begin(), vd.end());
    print(s3);
    std::complex<double> cd{2.4, 4.2};
    print(cd);
    print(cd * 6.0);
    print(cd += cd);
    print(std::pow(cd, 2));
    print(cd / cd);

    // Random Numbers
    // using RandomEngine = std::default_random_engine;
    using RandomEngine = std::mt19937_64;  // tipo do motor de probabilidade
    using UniformIntegerDistribution =
        std::uniform_int_distribution<int>;  // tipo de distribuição
    auto rd = std::random_device{};  // obter uma seed de forma aleatória para
                                     // cada execução
    RandomEngine r_eng{
        rd()};  // construção do objeto que será o motor de probabilidade, por
                // meio de um valor dado pelo 'random_device'.
    UniformIntegerDistribution dice_6{
        1, 6};  // um objetos que representa uma distribuição uniforme de
                // inteiros entre os valores [1:6];
    auto roll = [&r_eng, &dice_6]() { return dice_6(r_eng); };
    print(roll(), " ", roll(), " ", roll());
    // a fim de facilitar o uso de criação e uso de variáveis aleatórias, para
    // os casos mais comuns, pode-se implementar classes específicas:
    auto ri = RandomInt{1, 6};
    print(ri(), " ", ri(), " ", ri());
    // agora para um exemplo com um pouco mais de sustância:
    int max = 9;
    RandomInt rih{0, max};
    std::vector<int> histogram(max + 1);
    for (int i = 0; i < 300; i++) {
        ++histogram[rih()];
    }
    for (size_t i = 0; i < histogram.size(); i++) {
        cout << i << "\t";
        for (auto _ : std::ranges::iota_view{0, histogram[i]}) {
            cout << "*";
        }
        cout << endl;
    }
    // o uso de 'seeds' é importante não apenas para realizar 'debugging',
    // mas como também para reproducibilidade de resultados.
    //
    // Vector Arithmetic
    // <valarray> disponibiliza uma estrutura de dados otimizada para operações
    // numéricas vetoriais. as operações comuns ('+', '*', '+=', '-', 'pow',
    // 'sqrt', etc) também são aplicáveis à essa estrutura de dados. significa
    // que tais operadores serão aplicadoes a cada um de seus elementos
    // individualmente:
    std::valarray<double> va1{1, 2, 3, 4};
    std::valarray<double> va2{5, 6, 7, 8};
    auto print_var = [](auto& v) {
        for (size_t i = 0; i < v.size(); i++) {
            cout << v[i] << " ";
        }
        cout << endl;
    };
    std::valarray<double> va3 = va1 * va2;
    print_var(va3);
    va3 += 3.0;
    va3 = std::sqrt(va3);
    print_var(va3);

    // Numeric Limits
    // <limits> provém classes que descrevem propriedades dos tipos embutidos:
    print(std::numeric_limits<int>::min(), "  ",
          std::numeric_limits<int>::max());
    print(std::numeric_limits<double>::min(), "  ",
          std::numeric_limits<double>::max());
    print(std::numeric_limits<char>::is_signed);
    // é possível ainda, descrever 'numeric_limits' para os próprios tipos.

    // Constants
    using namespace std::numbers;  // namespace para todas as constantes
                                   // matemáticas.
    double p = pi * std::pow(3.0, 2);
    float f = pi_v<float> * std::pow(3.0, 2);
    print(p, " ", f);
    print(e, " ", log2e, " ", log10e, " ", pi, " ", inv_pi, " ", egamma, " ",
          phi, " ", sqrt2, " ", sqrt3, " ", ln2, " ", ln10);
    // ainda, é possível definir outras contantes, à partir das mesmas providas
    // pela 'stl'.
    // double tau = tau_v<double>;
    print(tau, " ", tau_v<float>);
};
}  // namespace capitulo_17
