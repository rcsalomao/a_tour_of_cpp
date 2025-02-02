#include <algorithm>
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

void print_iterable(auto&& iterable) {
    for (auto& x : iterable) {
        std::cout << x << " ";
    }
    std::cout << std::endl;
}

namespace capitulo_14 {
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using namespace std::ranges;

// uma 'range' pode ser definida como:
// {begin, end} -> par de iteradores;
// {begin, n} -> em que 'begin' é um interador e 'n' é o número de elementos;
// {begin, pred} -> em que 'begin' é um interador e 'pred' é um predicado. se
// 'pred(p)' for 'true', para o iterador 'p', então o fim da 'range' é
// alcançado. desta forma pode-se construir 'ranges' infinitas e 'ranges'
// construídas localmente;
//
// uma simples implementação de um algoritmo que faz uso de 'range' ao invés dos
// iteradores 'begin()' e 'end()':
template <forward_range R>
    requires std::sortable<iterator_t<R>>
void range_sort(R& r) {
    return std::sort(r.begin(), r.end());
}
// ainda, existem diversos tipos de 'ranges', representadas como 'concepts':
//
// input_range
// forward_range
// bidirection_range
// random_access_range
// contiguous_range

// View:
// 'views' são uma maneira de se observar elementos e trabalhar com 'ranges':
void use_view_1(forward_range auto& r) {
    // filter_view fv{r, [](int x) {
    //                    return x % 2;
    //                }};  // mostra apenas os valores ímpares de 'r'
    // take_view tv{fv, 3};  // mostra apenas os 'n' primeiros valores,
    //                       // filtrados anteriormente.
    // for (int x : tv) {
    //     cout << x << " ";
    // }
    // podemos também, evitar de difinir exclusivamente 'take_view' e
    // 'filter_view' da seguinte forma:
    for (int x : take_view{filter_view{r, [](int x) { return x % 2; }}, 3}) {
        cout << x << " ";
    }
    cout << endl;
}
// tais 'views' são denominadas 'range adapters':
// v = all_view{r}
// v = filter_view{r, p}
// v = transform_view{r, f}
// v = take_view{r, n}
// v = take_while_view{r, p}: 'v' representa elementos de 'r'
//                            até que algum deles não satisfaça 'p'
// v = drop_view{r, n}
// v = drop_while_view{r, p}: 'v' representa elementos de 'r'
//                            a partir do primeiro elemento que satisfaça 'p'
// v = join_view{r}
// v = split_view(r, d)
// v = common_view(r)
// v = reverse_view{r}
// v = views::elements<n>(r): 'v' é a 'range' dos elementos<n> ('get<n>')
//                            dos elemenentos do tipo 'tuple' de 'r'
// v = keys_view{r}: 'v' é a 'range' dos elementos 'first'
//                   dos elementos do tipo 'pair' de 'r'
// v = values_view{r}: 'v' é a 'range' dos elementos 'second'
//                     dos elementos do tipo 'pair' de 'r'
// v = ref_view{r}
// v = enumerate_view{r}: produz uma view de tuplas ({i, &e}).
// em que 'v' é uma view; 'r' é uma range; 'p' é um predicado e 'n' é um inteiro
// a api das 'views' é muito similar à api das 'ranges' e portanto é possível
// fazer uso das views nas mesmas situações em que se utilizaria uma 'range'
// a diferença é que uma 'view' não possue a posse dos elementos que esta
// mostra. uma 'view' não possui a responsabilidade de deletar os elementos.
auto mau_uso_view() {
    std::vector<int> v{1, 2, 3};
    return take_view{v, 3};  // 'v' será destruído antes da 'view'
}

// Generators:
// 'generators' permitem a criação de de 'ranges' 'on the fly'.
// alguns dos 'generators' providos pela 'stl' são:
// v = empty_view<T>{}
// v = single_view{x}
// v = iota_view{x}: 'v' é uma 'range' infinita composta pelos elementos
//                   x, x+1, x+2, x+n, .... O incremento é realizado
//                   pelo operador '++'.
// v = iota_view{x, y}: 'v' é uma 'range' finita composta pelos elementos
//                      x, x+1, x+2, y-1. O incremento é realizado
//                      pelo operador '++'.
// v = istream_view<T>{is}: 'v' é uma 'range' obtida por meio do operador '>>'
//                          para 'T' sobre 'is'.
// em que 'v' é uma 'view'; 'x' é um elemento do tipo 'T'; 'is' é um 'istream'.

// Pipes:
// por meio do operador pipeline '|' e possível repassar o output de uma função
// como input de outra e desta forma realizar concatenação de operações de forma
// sucinta e legível:
void use_view_2(forward_range auto& r) {
    for (int x :
         r | views::filter([](int x) { return x % 2; }) | views::take(3)) {
        cout << x << " ";
    }
    cout << endl;
}

// Concepts:
//
// conceitos relacionados a propriedades dos tipos e relações destes tipos para
// outros tipos refletem a variedade de tipos. os conceitos a seguir ajudam na
// simplificação de boa parte das templates:
// same_as<T, U>
// derived_from<T, U>
// convertible_to<T, U>
// common_reference_with<T, U>
// common_with<T, U>
// integral<T>
// signed_integral<T>
// unsigned_integral<T>
// floating_point<T>
// assignable_from<T, U>
// swappable_with<T, U>
// swappable<T>
//
// já os conceitos relacionados a comparação são:
// equality_comparable_with<T, U>
// equality_comparable<T>
// totally_ordered_with<T, U>
// totally_ordered<T>
// three_way_comparable_with<T, U>
// three_way_comparable<T>
//
// para a classificação dos tipos:
// destructible<T>
// constructible_from<T, Args>
// default_initializable<T>
// move_constructible<T>
// copy_constructible<T>
// movable<T>: move_constructible<T>, assignable<T&, T> e swappable<T>
// copyable<T>: copy_constructible<T>, movable<T> e assignable<T, const T&>
// semiregular<T>: copyable<T> e default_initializable<T>
// regular<T>: semiregular<T> e equality_comparable<T>
//
// para o caso de ser necessário repassar uma operação como argumento de
// template, deseja-se realizar restrições e suposições sobre como deve ser a
// semântica de invocação do tipo chamável:
// invocable<F, Args>
// regular_invocable<F, Args>: invocable<F, Args> e garante a equalidade
// predicate<F, Args>: regular_invocable<F, Args> e retorna um 'bool'
// relation<F, T, U>: predicate<F, T, U>
// equivalence_relation<F, T, U>
// strict_weak_order<F, T, U>
// em que 'equality preserving' significa que para uma função 'f()', se 'x == y'
// então 'f(x) == f(y)'
//
// para o caso de iteradores, as concepts para classificá-los são:
// input_or_ouput_iterator<I>: 'I' possui operadores '++' e '*'
// sentinel_for<S, I>: 'S' é um predicado ao valor de 'I'
// sized_sentinel_for<S, I>: 'S' é um sentinel e
//                           o operador '-' pode ser aplicado sobre 'I'
// input_iterator<I>
// output_iterator<I>
// forward_iterator<I>
// bidirectional_iterator<I>: forward_iterator<I> e suporta '--'
// random_access_iterator<I>: bidirectional_iterator<I>
//                            e suporta '+', '-', '+=', '-=' e '[]'
// contiguous_iterator<I>: random_access_iterator<I> para
//                         elementos contiguous em memória
// permutable<I>: forward_iterator<I> e suporta 'move' e 'swap'
// mergeable<I1, I2, R, O>: pode fundir sequencias definidas por
//                          'I1' e 'I2' em 'O' usando 'relation<R>'?
// sortable<I>: pode ordenar sequencias definidas por 'I' usando '<'?
// sortable<I, R>: pode ordenar sequencias definidas por
//                 'I' usando 'relation<R>'?
// a idéia básica de um 'sentinel' é realizar a iteração de um container de um
// ponto definido por um iterador 'p' até o momento em que um predicado retorne
// 'true', definindo assim uma 'range' como [p : s(*p)).
//
// por fim, 'range concepts' definem as propriedades de ranges:
// range<R>: 'R' é uma range com iteradores 'begin()' e 'sentinel'
// sized_range<R>: 'R' é uma range que sabe seu tamanho em tempo de compilação
// view<R>: 'R' é uma range com 'constant time' copy, move e assignment.
// common_range<R>: 'R' possui iteradores e 'sentinel' de tipos idênticos.
// input_range<R>: o tipo do iterador da range 'R' satisfaz 'input_iterator'
// output_range<R>: o tipo do iterador da range 'R' satisfaz 'output_iterator'
// forward_range<R>: o tipo do iterador da range 'R' satisfaz 'forward_iterator'
// bidirection_range<R>: o tipo do iterador da range 'R' satisfaz
//                       'bidirectional_iterator'
// random_access_range<R>: o tipo do iterador da range 'R' satisfaz
//                         'random_access_iterator'
// contiguous_range<R>: o tipo do iterador da range 'R' satisfaz
//                      'contiguous_iterator'

// a apresentação destes conceitos não é exaustiva, podendo-se encontrar mais
// definições na 'stl::ranges'. O uso principal destes conceitos é poder
// realizar sobrecarga em implementações, baseada nas propriedades dos tipos dos
// argumentos de entrada.

void main() {
    auto generate_vi = [](int i, int init_value = 0) {
        std::vector<int> vi(i);
        iota(vi, init_value);
        return vi;
    };
    auto vi1 = generate_vi(10);
    for (auto& x : vi1) {
        cout << x << " ";
    }
    cout << endl;
    use_view_1(vi1);
    // for (auto& x : vi1 | views::filter([](int x) { return x % 2 == 0; })) {
    //     cout << x << " ";
    // }
    // cout << endl;
    std::vector<std::vector<int>> vis{
        generate_vi(3),
        generate_vi(9),
        generate_vi(2),
    };
    std::vector<int> flat_vis;
    for (auto& x : join_view{vis}) {
        flat_vis.push_back(x);
    }
    for (auto& x : flat_vis) {
        cout << x << " ";
    }
    cout << endl;

    // generators:
    for (int x : iota_view(42, 52)) {
        cout << x << " ";
    }
    cout << endl;
    // pipes:
    use_view_2(vi1);
    for (int x :
         views::iota(42, 52) | views::transform([](int i) { return i % 3; })) {
        cout << x << " ";
    }
    cout << endl;
};
}  // namespace capitulo_14
