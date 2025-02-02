#include <algorithm>
#include <any>
#include <bit>
#include <cassert>
#include <chrono>
#include <concepts>
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
#include <variant>
#include <vector>

namespace capitulo_16 {
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

struct Entry {
    string name;
    int number;

    bool operator==(const Entry& e) const {
        return (name == e.name) && (number == e.number);
    }

    bool operator<(const Entry& y) const { return name < y.name; }

    void print_name() { cout << name << endl; }
};

int f1(double d) { return 2 * d; };
int f2(string s) { return s.length(); };

// Type Predicates
// encontrados á partir da biblioteca <type_traits> e servem para responder
// perguntas fundamentais sobre tipos:
namespace std_like {
// helper class
template <class T, T v>
struct integral_constant;

template <bool B>
using bool_constant = integral_constant<bool, B>;
using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

// primary type categories
template <class T>
struct is_void;
template <class T>
struct is_null_pointer;
template <class T>
struct is_integral;
template <class T>
struct is_floating_point;
template <class T>
struct is_array;
template <class T>
struct is_pointer;
template <class T>
struct is_lvalue_reference;
template <class T>
struct is_rvalue_reference;
template <class T>
struct is_member_object_pointer;
template <class T>
struct is_member_function_pointer;
template <class T>
struct is_enum;
template <class T>
struct is_union;
template <class T>
struct is_class;
template <class T>
struct is_function;

// composite type categories
template <class T>
struct is_reference;
template <class T>
struct is_arithmetic;
template <class T>
struct is_fundamental;
template <class T>
struct is_object;
template <class T>
struct is_scalar;
template <class T>
struct is_compound;
template <class T>
struct is_member_pointer;

// type properties
template <class T>
struct is_const;
template <class T>
struct is_volatile;
template <class T>
struct is_trivial;
template <class T>
struct is_trivially_copyable;
template <class T>
struct is_standard_layout;
template <class T>
struct is_empty;
template <class T>
struct is_polymorphic;
template <class T>
struct is_abstract;
template <class T>
struct is_final;
template <class T>
struct is_aggregate;

template <class T>
struct is_signed;
template <class T>
struct is_unsigned;
template <class T>
struct is_bounded_array;
template <class T>
struct is_unbounded_array;
template <class T>
struct is_scoped_enum;

template <class T, class... Args>
struct is_constructible;
template <class T>
struct is_default_constructible;
template <class T>
struct is_copy_constructible;
template <class T>
struct is_move_constructible;

template <class T, class U>
struct is_assignable;
template <class T>
struct is_copy_assignable;
template <class T>
struct is_move_assignable;

template <class T, class U>
struct is_swappable_with;
template <class T>
struct is_swappable;

template <class T>
struct is_destructible;

template <class T, class... Args>
struct is_trivially_constructible;
template <class T>
struct is_trivially_default_constructible;
template <class T>
struct is_trivially_copy_constructible;
template <class T>
struct is_trivially_move_constructible;

template <class T, class U>
struct is_trivially_assignable;
template <class T>
struct is_trivially_copy_assignable;
template <class T>
struct is_trivially_move_assignable;
template <class T>
struct is_trivially_destructible;

template <class T, class... Args>
struct is_nothrow_constructible;
template <class T>
struct is_nothrow_default_constructible;
template <class T>
struct is_nothrow_copy_constructible;
template <class T>
struct is_nothrow_move_constructible;

template <class T, class U>
struct is_nothrow_assignable;
template <class T>
struct is_nothrow_copy_assignable;
template <class T>
struct is_nothrow_move_assignable;

template <class T, class U>
struct is_nothrow_swappable_with;
template <class T>
struct is_nothrow_swappable;

template <class T>
struct is_nothrow_destructible;

template <class T>
struct has_virtual_destructor;

template <class T>
struct has_unique_object_representations;

template <class T, class U>
struct reference_constructs_from_temporary;
template <class T, class U>
struct reference_converts_from_temporary;

// type property queries
template <class T>
struct alignment_of;
template <class T>
struct rank;
template <class T, unsigned I = 0>
struct extent;

// type relations
template <class T, class U>
struct is_same;
template <class Base, class Derived>
struct is_base_of;
template <class Base, class Derived>
struct is_virtual_base_of;
template <class From, class To>
struct is_convertible;
template <class From, class To>
struct is_nothrow_convertible;
template <class T, class U>
struct is_layout_compatible;
template <class Base, class Derived>
struct is_pointer_interconvertible_base_of;

template <class Fn, class... ArgTypes>
struct is_invocable;
template <class R, class Fn, class... ArgTypes>
struct is_invocable_r;

template <class Fn, class... ArgTypes>
struct is_nothrow_invocable;
template <class R, class Fn, class... ArgTypes>
struct is_nothrow_invocable_r;

// const-volatile modifications
template <class T>
struct remove_const;
template <class T>
struct remove_volatile;
template <class T>
struct remove_cv;
template <class T>
struct add_const;
template <class T>
struct add_volatile;
template <class T>
struct add_cv;

template <class T>
using remove_const_t = typename remove_const<T>::type;
template <class T>
using remove_volatile_t = typename remove_volatile<T>::type;
template <class T>
using remove_cv_t = typename remove_cv<T>::type;
template <class T>
using add_const_t = typename add_const<T>::type;
template <class T>
using add_volatile_t = typename add_volatile<T>::type;
template <class T>
using add_cv_t = typename add_cv<T>::type;

// reference modifications
template <class T>
struct remove_reference;
template <class T>
struct add_lvalue_reference;
template <class T>
struct add_rvalue_reference;

template <class T>
using remove_reference_t = typename remove_reference<T>::type;
template <class T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;
template <class T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

// sign modifications
template <class T>
struct make_signed;
template <class T>
struct make_unsigned;

template <class T>
using make_signed_t = typename make_signed<T>::type;
template <class T>
using make_unsigned_t = typename make_unsigned<T>::type;

// array modifications
template <class T>
struct remove_extent;
template <class T>
struct remove_all_extents;

template <class T>
using remove_extent_t = typename remove_extent<T>::type;
template <class T>
using remove_all_extents_t = typename remove_all_extents<T>::type;

// pointer modifications
template <class T>
struct remove_pointer;
template <class T>
struct add_pointer;

template <class T>
using remove_pointer_t = typename remove_pointer<T>::type;
template <class T>
using add_pointer_t = typename add_pointer<T>::type;

// other transformations
template <class T>
struct type_identity;
template <class T>
struct remove_cvref;
template <class T>
struct decay;
template <bool, class T = void>
struct enable_if;
template <bool, class T, class F>
struct conditional;
template <class... T>
struct common_type;
template <class T, class U, template <class> class TQual,
          template <class> class UQual>
struct basic_common_reference {};
template <class... T>
struct common_reference;
template <class T>
struct underlying_type;
template <class Fn, class... ArgTypes>
struct invoke_result;
template <class T>
struct unwrap_reference;
template <class T>
struct unwrap_ref_decay;

template <class T>
using type_identity_t = typename type_identity<T>::type;
template <class T>
using remove_cvref_t = typename remove_cvref<T>::type;
template <class T>
using decay_t = typename decay<T>::type;
template <bool b, class T = void>
using enable_if_t = typename enable_if<b, T>::type;
template <bool b, class T, class F>
using conditional_t = typename conditional<b, T, F>::type;
template <class... T>
using common_type_t = typename common_type<T...>::type;
template <class... T>
using common_reference_t = typename common_reference<T...>::type;
template <class T>
using underlying_type_t = typename underlying_type<T>::type;
template <class Fn, class... ArgTypes>
using invoke_result_t = typename invoke_result<Fn, ArgTypes...>::type;
template <class T>
using unwrap_reference_t = typename unwrap_reference<T>::type;
template <class T>
using unwrap_ref_decay_t = typename unwrap_ref_decay<T>::type;
template <class...>
using void_t = void;

// logical operator traits
template <class... B>
struct conjunction;
template <class... B>
struct disjunction;
template <class B>
struct negation;

// primary type categories
template <class T>
inline constexpr bool is_void_v = is_void<T>::value;
template <class T>
inline constexpr bool is_null_pointer_v = is_null_pointer<T>::value;
template <class T>
inline constexpr bool is_integral_v = is_integral<T>::value;
template <class T>
inline constexpr bool is_floating_point_v = is_floating_point<T>::value;
template <class T>
inline constexpr bool is_array_v = is_array<T>::value;
template <class T>
inline constexpr bool is_pointer_v = is_pointer<T>::value;
template <class T>
inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;
template <class T>
inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;
template <class T>
inline constexpr bool is_member_object_pointer_v =
    is_member_object_pointer<T>::value;
template <class T>
inline constexpr bool is_member_function_pointer_v =
    is_member_function_pointer<T>::value;
template <class T>
inline constexpr bool is_enum_v = is_enum<T>::value;
template <class T>
inline constexpr bool is_union_v = is_union<T>::value;
template <class T>
inline constexpr bool is_class_v = is_class<T>::value;
template <class T>
inline constexpr bool is_function_v = is_function<T>::value;

// composite type categories
template <class T>
inline constexpr bool is_reference_v = is_reference<T>::value;
template <class T>
inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;
template <class T>
inline constexpr bool is_fundamental_v = is_fundamental<T>::value;
template <class T>
inline constexpr bool is_object_v = is_object<T>::value;
template <class T>
inline constexpr bool is_scalar_v = is_scalar<T>::value;
template <class T>
inline constexpr bool is_compound_v = is_compound<T>::value;
template <class T>
inline constexpr bool is_member_pointer_v = is_member_pointer<T>::value;

// type properties
template <class T>
inline constexpr bool is_const_v = is_const<T>::value;
template <class T>
inline constexpr bool is_volatile_v = is_volatile<T>::value;
template <class T>
inline constexpr bool is_trivial_v = is_trivial<T>::value;
template <class T>
inline constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;
template <class T>
inline constexpr bool is_standard_layout_v = is_standard_layout<T>::value;
template <class T>
inline constexpr bool is_empty_v = is_empty<T>::value;
template <class T>
inline constexpr bool is_polymorphic_v = is_polymorphic<T>::value;
template <class T>
inline constexpr bool is_abstract_v = is_abstract<T>::value;
template <class T>
inline constexpr bool is_final_v = is_final<T>::value;
template <class T>
inline constexpr bool is_aggregate_v = is_aggregate<T>::value;
template <class T>
inline constexpr bool is_signed_v = is_signed<T>::value;
template <class T>
inline constexpr bool is_unsigned_v = is_unsigned<T>::value;
template <class T>
inline constexpr bool is_bounded_array_v = is_bounded_array<T>::value;
template <class T>
inline constexpr bool is_unbounded_array_v = is_unbounded_array<T>::value;
template <class T>
inline constexpr bool is_scoped_enum_v = is_scoped_enum<T>::value;
template <class T, class... Args>
inline constexpr bool is_constructible_v = is_constructible<T, Args...>::value;
template <class T>
inline constexpr bool is_default_constructible_v =
    is_default_constructible<T>::value;
template <class T>
inline constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;
template <class T>
inline constexpr bool is_move_constructible_v = is_move_constructible<T>::value;
template <class T, class U>
inline constexpr bool is_assignable_v = is_assignable<T, U>::value;
template <class T>
inline constexpr bool is_copy_assignable_v = is_copy_assignable<T>::value;
template <class T>
inline constexpr bool is_move_assignable_v = is_move_assignable<T>::value;
template <class T, class U>
inline constexpr bool is_swappable_with_v = is_swappable_with<T, U>::value;
template <class T>
inline constexpr bool is_swappable_v = is_swappable<T>::value;
template <class T>
inline constexpr bool is_destructible_v = is_destructible<T>::value;
template <class T, class... Args>
inline constexpr bool is_trivially_constructible_v =
    is_trivially_constructible<T, Args...>::value;
template <class T>
inline constexpr bool is_trivially_default_constructible_v =
    is_trivially_default_constructible<T>::value;
template <class T>
inline constexpr bool is_trivially_copy_constructible_v =
    is_trivially_copy_constructible<T>::value;
template <class T>
inline constexpr bool is_trivially_move_constructible_v =
    is_trivially_move_constructible<T>::value;
template <class T, class U>
inline constexpr bool is_trivially_assignable_v =
    is_trivially_assignable<T, U>::value;
template <class T>
inline constexpr bool is_trivially_copy_assignable_v =
    is_trivially_copy_assignable<T>::value;
template <class T>
inline constexpr bool is_trivially_move_assignable_v =
    is_trivially_move_assignable<T>::value;
template <class T>
inline constexpr bool is_trivially_destructible_v =
    is_trivially_destructible<T>::value;
template <class T, class... Args>
inline constexpr bool is_nothrow_constructible_v =
    is_nothrow_constructible<T, Args...>::value;
template <class T>
inline constexpr bool is_nothrow_default_constructible_v =
    is_nothrow_default_constructible<T>::value;
template <class T>
inline constexpr bool is_nothrow_copy_constructible_v =
    is_nothrow_copy_constructible<T>::value;
template <class T>
inline constexpr bool is_nothrow_move_constructible_v =
    is_nothrow_move_constructible<T>::value;
template <class T, class U>
inline constexpr bool is_nothrow_assignable_v =
    is_nothrow_assignable<T, U>::value;
template <class T>
inline constexpr bool is_nothrow_copy_assignable_v =
    is_nothrow_copy_assignable<T>::value;
template <class T>
inline constexpr bool is_nothrow_move_assignable_v =
    is_nothrow_move_assignable<T>::value;
template <class T, class U>
inline constexpr bool is_nothrow_swappable_with_v =
    is_nothrow_swappable_with<T, U>::value;
template <class T>
inline constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<T>::value;
template <class T>
inline constexpr bool is_nothrow_destructible_v =
    is_nothrow_destructible<T>::value;
template <class T>
inline constexpr bool has_virtual_destructor_v =
    has_virtual_destructor<T>::value;
template <class T>
inline constexpr bool has_unique_object_representations_v =
    has_unique_object_representations<T>::value;
template <class T, class U>
inline constexpr bool reference_constructs_from_temporary_v =
    reference_constructs_from_temporary<T, U>::value;
template <class T, class U>
inline constexpr bool reference_converts_from_temporary_v =
    reference_converts_from_temporary<T, U>::value;

// type property queries
template <class T>
inline constexpr size_t alignment_of_v = alignment_of<T>::value;
template <class T>
inline constexpr size_t rank_v = rank<T>::value;
template <class T, unsigned I = 0>
inline constexpr size_t extent_v = extent<T, I>::value;

// type relations
template <class T, class U>
inline constexpr bool is_same_v = is_same<T, U>::value;
template <class Base, class Derived>
inline constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;
template <class Base, class Derived>
inline constexpr bool is_virtual_base_of_v =
    is_virtual_base_of<Base, Derived>::value;
template <class From, class To>
inline constexpr bool is_convertible_v = is_convertible<From, To>::value;
template <class From, class To>
inline constexpr bool is_nothrow_convertible_v =
    is_nothrow_convertible<From, To>::value;
template <class T, class U>
inline constexpr bool is_layout_compatible_v =
    is_layout_compatible<T, U>::value;
template <class Base, class Derived>
inline constexpr bool is_pointer_interconvertible_base_of_v =
    is_pointer_interconvertible_base_of<Base, Derived>::value;
template <class Fn, class... ArgTypes>
inline constexpr bool is_invocable_v = is_invocable<Fn, ArgTypes...>::value;
template <class R, class Fn, class... ArgTypes>
inline constexpr bool is_invocable_r_v =
    is_invocable_r<R, Fn, ArgTypes...>::value;
template <class Fn, class... ArgTypes>
inline constexpr bool is_nothrow_invocable_v =
    is_nothrow_invocable<Fn, ArgTypes...>::value;
template <class R, class Fn, class... ArgTypes>
inline constexpr bool is_nothrow_invocable_r_v =
    is_nothrow_invocable_r<R, Fn, ArgTypes...>::value;

// logical operator traits
template <class... B>
inline constexpr bool conjunction_v = conjunction<B...>::value;
template <class... B>
inline constexpr bool disjunction_v = disjunction<B...>::value;
template <class B>
inline constexpr bool negation_v = negation<B>::value;

// member relationships
template <class S, class M>
constexpr bool is_pointer_interconvertible_with_class(M S::*m) noexcept;
template <class S1, class S2, class M1, class M2>
constexpr bool is_corresponding_member(M1 S1::*m1, M2 S2::*m2) noexcept;

// constant evaluation context
constexpr bool is_constant_evaluated() noexcept;
consteval bool is_within_lifetime(const auto*) noexcept;
}  // namespace std_like
// um uso comum é realizar a restrição de argumentos em templates:
template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;
template <Arithmetic Scalar>
class complex {
    Scalar re, im;

   public:
    //...
};
// muitos dos 'type predicates' são aplicáveis apenas aos tipos embutidos.
// entretanto é possível, por meio de concepts, criar o próprio predicado para
// um caso mais geral:
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
template <typename T, typename U>
concept Arithmetic2 = Number<T, U> && Number<U, T>;

// Conditional Properties
// é possível fazer uso de type predicate e concepts para aplicar restrições
// sobre quais tipos de operações são possíveis ou não em determinados
// contextos:
template <typename T>
class SmartPointer {
    //...
    T& operator*() const;
    T* operator->() const
        requires std::is_class_v<T>
    ;  // o operador '->' é definido APENAS se 'T' for uma classe
};

template <typename T>
concept Class = std::is_class_v<T> ||
                std::is_union_v<T>;  // conceito para classes ou 'union'
template <typename T>
class SmartPointer2 {
    //...
    T& operator*() const;
    T* operator->() const
        requires Class<T>;
};

// Type Generators
// das 'type functions', algumas são capazes de calcular e retornar novos tipos.
template <typename T>
class SmartPointer3 {
    //...
    T& operator*() const;
    std::enable_if<std::is_class_v<T>, T&>
    operator->();  // operador '->' é definido APENAS se 'T' for uma classe
};

// Associated Types
// todos os containers da 'stl' foram desenvolvidos de forma a seguir um padrão
// de possuir 'tipos associados', envolvendo os tipos dos valores dos elementos
// e iteradores. em <iterator> e <ranges> a 'stl' provém:
// range_value_t<R>: o tipo dos elementos da range 'R'.
// iter_value<T>: os tipos dos elementos apontados por um iterador 'T'.
// iterator_t<R>: o tipo do iterador de uma range 'R'.

template <typename T>
void swap(T& a, T& b) {
    T tmp{std::move(a)};
    a = std::move(b);
    b = std::move(tmp);
}

template <typename T, typename... Args>
std::unique_ptr<T> faz_unico(Args&&... args) {
    return std::unique_ptr<T>{new T{std::forward<Args>(args)...}};
}

void main() {
    // Clocks
    auto t0 = system_clock::now();  // tipo: time_point
    std::this_thread::sleep_for(
        10ms + 33us);  // à partir da biblioteca <thread>. 'sleep_for' aceita
                       // argumentos de entrada do tipo chrono::duration.
    auto t1 = system_clock::now();  // tipo: time_point
    auto duration = t1 - t0;        // tipo: duration
    cout << duration << endl;
    cout << duration_cast<milliseconds>(t1 - t0).count() << "ms" << endl;
    cout << duration_cast<nanoseconds>(t1 - t0).count() << "ns" << endl;

    // Calendars
    auto day = April / 7 / 2018;  // day = std::chorno::year_month_day
                                  // std::chrono::April = std::chorno::month
    cout << day << endl;
    cout << weekday(day) << endl;
    print(format("{:%A}", day));
    auto day_2 =
        2018y / April / 7;  // para especificar a data em outro formato (ordem),
                            // deve-se especificar o tipo do primeiro argumento
                            // (se é do tipo 'dia' ou 'ano').
    auto day_3 = 7d / April / 2018;
    cout << day_2 << endl;
    cout << day_3 << endl;
    // ainda assim, é possível acabar definindo dias inválidos. para tanto a
    // biblioteca provém o método 'ok()' para verificar se a data criada é
    // válida.
    auto dia_ruim = January / 0 / 2024;
    if (!dia_ruim.ok()) {
        print(dia_ruim);  // parece que na operação de '<<' ele já verifica e
                          // mostra que a data não é válida.
    }
    // a biblioteca oferece métodos e possibilidade de conversão entre os tipos
    // 'time_point' e 'year_month_day'.
    auto t = sys_days{February / 25 /
                      2025};     // obter um 'time_point' com precisão de dias
    t += days{7};                // uma semana à partir da data anterior.
    auto d = year_month_day{t};  // converter o objeto do tipo 'time_point' para
                                 // 'year_month_day' (calendar).
    print(d);
    print(format("{:%B}/{}/{}", d.month(), d.day(), d.year()));

    // Timezones
    time_point tp = system_clock::now();
    print(tp);
    zoned_time ztp{current_zone(), tp};
    print(ztp);
    print(current_zone()->name());

    // Function Adaptors
    //
    // lambdas:
    // assim como em todos os algoritmos da 'stl', 'for_each()' chama seus
    // argumentos empregando a forma tracional de função 'f(x)', entretando em
    // algumas situações a interface exige a forma convencional orientada a
    // objeto 'x->f()'. lambdas permitem que se criem adaptadores no local para
    // compatibilização:
    std::vector<std::unique_ptr<Entry>> ve;
    ve.emplace_back(std::make_unique<Entry>("Sarah Connor"s, 123));
    ve.emplace_back(std::make_unique<Entry>("Zabuza Momochi"s, 456));
    ve.emplace_back(std::make_unique<Entry>("Miyamoto Musashi"s, 789));
    std::ranges::for_each(ve, [](auto&& e) { e->print_name(); });
    // mem_fn():
    // dada uma função membro de uma classe, 'mem_fn(mf)' produz um objeto
    // função que pode ser chamado como uma função não membro:
    std::ranges::for_each(ve, std::mem_fn(&Entry::print_name));
    // function:
    // o objeto 'function' da 'stl' é um objeto que pode ser chamado pelo
    // operador '()':
    std::function<int(double)> fct1{f1};  // inicialização para 'f1'
    std::function fct2{f2};  // dedução do tipo de fct2 por meio de 'f2'
    std::function fct3 = [](Entry& e) {
        e.print_name();
    };  // dedução do tipo por meio da função lambda.
    // funções são úteis como 'callbacks', operações como argumentos, functors,
    // etc. entretanto, pode em certos casos comprometer em termos de
    // performance e tamanho. outra questão é que, por 'std::function' ser um
    // objeto, este não participa do mecanismo de sobrecarga (assim como
    // lambdas), restando fazer uso da função template 'overload' (capítulo 15).
    //
    // Type Functions
    // uma 'type function' é uma função que é avaliada em tempo de compilação,
    // recebendo um tipo como argumento ou retornando um tipo. por exemplo, da
    // biblioteca <limits>:
    constexpr double min =
        std::numeric_limits<double>::min();  // menor valor positivo do tipo
                                             // 'double'.
    print(min);
    constexpr int sz_i = sizeof(int);  // o tamanho do tipo 'int' em bytes.
    print(sz_i);
    // em <type_traits>, a 'stl' provém uma gama de funções para realizar a
    // consulta de propriedades dos tipos.
    // bool b = std::is_arithmetic<double>::value;
    // bool b = std::is_arithmetic_v<double>;
    bool b = std::is_arithmetic<double>();
    print(b);
    // 'concepts' pondem ser citados como um exemplo de 'type functions'.
    // adicionalmente, das 'type functions' da 'stl', a convenção é que seus
    // nomes terminem com '_v' caso essas funções retornem valores e '_t' caso
    // elas retornem tipos.
    //
    // Source Location
    // A 'stl' provém no namespace 'source_location' funcionalidade para
    // reportar informação referente à posição no código fonte.
    const std::source_location loc = std::source_location::current();
    cout << loc.file_name() << endl
         << loc.function_name() << endl
         << loc.line() << endl
         << loc.column() << endl;
    // 'move()' e 'forward()'
    // de forma geral, o compilador possui a prerrogativa de decidir se
    // determinado objeto será copiado ou movido. entretanto com o uso das
    // funções 'move' e 'forward', é possível indicar para o compilador qual
    // operação utilizar.
    auto p = std::make_unique<int>(2);
    // auto q = p;  // erro: não é possível realizar cópia de um 'unique_ptr'.
    auto q = std::move(p);  // agora 'p' aponta para 'nullptr'.
    // na realidade, 'move()' não move, mas apenas faz um cast para 'rvalue', e
    // portanto, fazendo com que o compilador execute a operação de 'move' no
    // objeto. 'move' é interessante quando não se quer realizar operações de
    // cópia. entretanto, deve-se tomar cuidada para não tentar acessar os
    // objetos que já foram movidos, pois se trata de 'undefined behavior'.
    string s1 = "Hello";
    string s2 = "World";
    std::vector<string> vs;
    vs.push_back(s1);  // faz uso de 'const string&' e portanto realiza cópia do
                       // elemento.
    vs.push_back(std::move(s2));  // usa um 'move constructor'.
    vs.emplace_back(s1);  // alternativamente, 'emplace_back' coloca uma cópia
                          // de 's1' na sua nova posição ao fim de 'vs'.
    cout << s1 << endl;   // ok
    // cout << s2 << endl;   // erro: não possui nada. 'ub'.
    // adicionalmente, quando se deseja passar os argumentos para frente,
    // deve-se usar 'std::forward<>'. 'std::forward<>' realiza a correta
    // distinção e tratamento entre tipos 'lvalue' e 'rvalue'.
    //
    // Bit Manipulation
    double val = 4.2;
    auto x =
        std::bit_cast<uint64_t>(val);  // obter a representação em bit de um
                                       // número de ponto flutuante em 64-bit.
    auto y = std::bit_cast<uint64_t>(
        &val);  // obter a representação de um ponteiro em 64-bit.
                //
    struct word {
        std::byte b[8];  // 'std::byte' existe para representar bytes. fornece
                         // apenas operações lógicas 'bit-wise'.
    };
    std::byte buffer[1024];
    // ...
    auto w = std::bit_cast<word*>(&buffer[0]);
    auto i = std::bit_cast<int64_t>(*w);

    // Exiting Program
    // Os mecanismos para provocar a saída de um programa podem ser:
    // exit(x): invoca as funções registradas com 'atexit()' e então sai do
    // programa e retorna o valor 'x'.
    // abort(): provoca a saída do programa imediatamente e incondicionalmente
    // com o valor de retorno indicando uma terminação sem sucesso.
    // quick_exit(x): invoca todas as funções registradas em 'at_quick_exit()',
    // então sai do programa retornando o valor 'x'.
    // terminate(): invoca o 'terminate_handler'. o valor padrão é 'abort()'.
    //
    // Tais funções são para erros sérios. não invocam destructors, nem realizam
    // operações de limpeza. Há diversos handlers para serem utilizados antes da
    // operação de saída, mas precisam ser bem simples, pois vale lembrar que só
    // há a necessidade de se terminar um programa quando um erro muito grave
    // tenha ocorrido.
};
}  // namespace capitulo_16
