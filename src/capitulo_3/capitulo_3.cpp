#include <format>
#include <print>
#include <unordered_map>
#include <variant>
#include <vector>

void print(auto&& v) { std::println("{}", v); }

namespace capitulo_3 {
int sum(const std::vector<int>&
            v) {  // argumento v é uma referência à um vetor externo ao escopo
                  // da função. Também é declarado como const para demonstrar
                  // que o mesmo não irá sofre mutação.
    int s = 0;
    for (const int i : v) {
        s += i;
    }
    return s;  // o resultado retornado é feito por cópia do valor inteiro para
               // o escopo invocador.
}

//  O comportamento padrão é passar os argumentos por cópia.
//  Entretanto há possibilidade de otimização por meio do compilador.

void test(std::vector<int> v,
          std::vector<int>& rv) {  // v é passado por valor (cópia). rv é
                                   // passado por referência (ponteiro).
    v[1] = 99;   // modificação de v que é uma variável local (cópia).
    rv[2] = 66;  // modificação de rv, que é uma variável não-local,
                 // representada por referência.
}

void print_num(int valor, int base = 10) {
    ;
}  // argumento 'base' possui um valor default. quando não informado na chamada
   // da respectiva função, assume este valor como padrão.
class Vector {
   public:
    Vector(int s)
        : elem{new double[s]}, sz{s} {};  // construtor da classe Vector
    double& operator[](
        int i) {  // o retorno deste operador 'double&' é uma referência ao
                  // 'iésimo' valor da array. Evita-se desta forma realizar
                  // cópias desnecessárias e também torna possível a atribuição
                  // específica dos valores da array.
        return elem[i];
    };  // acesso aos elementos pelo operador '[]'
    int size() { return sz; };

   private:
    double* elem;  // ponteiro para os elementos 'elem'
    int sz;        // número de elementos
};

// int& bad() {
//     int x;
//     return x;  // ruim: irá retornar uma referência de uma variável local da
//                // função. Variáveis locais são destruídas no momento que se
//                // sai do escopo d função e se retorna para o escopo de
//                // invocação.
// }

// para o caso de retorno de uma função por cópia. De forma geral é possível
// para o compilador realizar otimizações, tais como 'Return Value Optimization
// (RVO)' que permitem a supressão da operação de cópia. O novo objeto de
// retorno já é construído no endereço de memória a ser retornado no escopo de
// invocação.

auto mul(int i, double d) {
    return i * d;
}  // com a instrução 'auto' é possível também realizar dedução automática do
   // tipo do objeto a ser retornado.

auto mul_2(int i, double d) -> double {
    return i * d;
}  // também é possível escrever desta forma, com o tipo de retorno de forma
   // sufixa. Desta maneira o tipo fica localizado após os argumentos de entrada
   // da função.

struct Entry {
    std::string name;
    int value;
};

Entry give_me_entry(std::string nome, int valor) { return {nome, valor}; }

class Box {
   public:
    Box() { print("Box default constructor"); };
    Box(double x) : m_x(x) { print("Box parameter initialized constructor"); }
    Box(const Box& other) : m_x(other.m_x) { print("Box copy constructor"); }
    Box& operator=(const Box& other) {
        print("Box copy assignment operator");
        m_x = other.m_x;
        return *this;
    }
    Box(const Box&& other) : m_x(std::move(other.m_x)) {
        print("Box move constructor");
    }
    Box& operator=(const Box&& other) {
        print("Box move assignment operator");
        m_x = std::move(other.m_x);
        return *this;
    }

    double x() { return m_x; }

   private:
    double m_x{0.0};
};

Box get_me_a_box_1() {
    Box b{4.1};
    return b;
}

Box get_me_a_box_2() {
    static Box b{4.2};
    return b;
}

Box& get_me_a_box_3() {
    static Box c{4.3};
    return c;
}

Box get_me_a_box_4() {
    Box b{4.4};
    return std::move(b);  // std::move, neste caso, impede que o compilador
                          // possa realizar otimizações, tais como RVO.
}

Box get_me_a_box_5() {
    Box b{4.5};
    return std::forward<decltype(b)>(b);
}

void main() {
    std::vector<int> fib = {1, 2, 3, 5, 8, 13, 21};
    test(fib, fib);
    print(std::format("{} {}", fib[1], fib[2]));
    auto [nome, valor] = give_me_entry("Abril", 24);
    print(std::format("nome: {}; valor: {}", nome, valor));
    std::unordered_map<std::string, int> m;
    m.insert({"Abril", 24});
    m.insert({{"Agosto", 10}, {"Novembro", 16}});
    for (auto& [chave, valor] : m) {
        print(std::format("mês: {}; dia: {}", chave, valor));
    }
    Box b{42};  // parameter initialized constructor
    print(b.x());
    Box c = {b};  // copy constructor
    print(c.x());
    Box d{};  // default constructor
    d = c;    // copy assignment constructor
    print(d.x());
    Box e{Box{2.4}};  // parameter initialized constructor
    print(e.x());
    Box f = std::move(e);  // move constructor
    print(f.x());
    print(e.x());
    Box g;             // default constructor
    g = std::move(f);  // move assignment constructor
    print(g.x());
    print(f.x());
    print("===============");
    Box h =
        get_me_a_box_1();  // parameter initialized constructor. copy elided?
    print(h.x());
    Box i = get_me_a_box_2();  // copy constructor
    print(i.x());
    Box j = {get_me_a_box_3()};  // copy constructor
    print(j.x());
    Box k = {get_me_a_box_4()};  // move constructor
    print(k.x());
    Box l = {get_me_a_box_5()};  // move constructor
    print(l.x());
    Box n;
    n = get_me_a_box_3();
    print(n.x());
}
}  // namespace capitulo_3
