#include <print>
#include <variant>

void print(auto&& v) { std::println("{}", v); }

namespace capitulo_2 {
struct Vector {
    double* elem;  // ponteiro para os elementos
    int sz;        // número de elementos
};

void vector_init(Vector& v, int s) {
    if (s <= 0) {
        return;
    }
    v.elem = new double[s];
    v.sz = s;
}

double sum(int s) {
    Vector v;
    vector_init(v, s);

    for (int i = 0; i < s; i++) {
        v.elem[i] = (i + 1) * 1.1;
    }
    double sum = 0;
    for (int i = 0; i < s; i++) {
        sum += v.elem[i];
    }
    return sum;
}

void f(Vector v, Vector& rv, Vector* pv) {
    int i1 =
        v.sz;  // acesso de membro, de uma variável local copy-init, pelo nome
    int i2 = rv.sz;   // acesso de membro de uma referência, pelo nome
    int i3 = pv->sz;  // acesso de membro por um ponteiro
}

class Vector_2 {
   public:
    Vector_2(int s)
        : elem{new double[s]}, sz{s} {};  // construtor da classe Vector_2
    double& operator[](int i) {
        return elem[i];
    };  // acesso aos elementos pelo operador '[]'
    int size() { return sz; };

   private:
    double* elem;  // ponteiro para os elementos 'elem'
    int sz;        // número de elementos
};

double sum_2(int s) {
    Vector_2 v{s};

    for (int i = 0; i < s; i++) {
        v[i] = (i + 1) * 1.1;
    }
    double sum = 0;
    for (int i = 0; i < s; i++) {
        sum += v[i];
    }
    return sum;
}

enum class Color { red, blue, green };
enum class TrafficLight { green, yellow, red };
TrafficLight& operator++(TrafficLight& t) {
    using enum TrafficLight;
    switch (t) {
        case green:
            return t = yellow;
            break;
        case yellow:
            return t = red;
            break;
        case red:
            return t = green;
            break;
    }
}

enum class Type { ptr, num };
struct Node {
    double x;
};
struct Entry {
    std::string name;
    Type t;
    Node* p;
    int i;
};
void f(Entry* pe) {
    if (pe->t == Type::num) {
        print(pe->i);
    }
}

union Value {
    Node* p;
    int i;
};
struct Entry_2 {
    std::string name;
    Type t;
    Value v;
};
void f(Entry_2* pe) {
    if (pe->t == Type::num) {
        print(pe->v.i);
    }
}

struct Entry_3 {
    std::string name;
    std::variant<Node*, int> v;
};
void f(Entry_3* pe) {
    if (std::holds_alternative<int>(pe->v)) {
        print(std::get<int>(pe->v));
    }
}

void main() {
    print(sum(5));
    {
        Vector v;
        vector_init(v, 10);
        f(v, v, &v);
    }
    print(sum_2(5));
    {
        Color color = Color::red;
        TrafficLight light = TrafficLight::red;
        // Color x1 = red;  // erro: cor red existe apenas dentro do contexto da
        //                    // classe Color.
        // Color x2 = TrafficLight::red;  // erro: Só é possível atribuir para
        //                                  // x2 as cores da classe Color,
        //                                  // independente do nome dos seus
        //                                  // respectivos membros.
        Color x1 = Color::red;
        auto x2 = TrafficLight::red;
        // int i = Color::red;  // erro: É vedada a conversão de um enum class
        //                        // para inteiro.
        // Color c = 2;  // erro: igualmente vedada a conversão de inteiro para
        //               // enum class.
        Color x = Color{5};  // Ainda assim, é possível realizar inicialização
                             // por meio de valores inteiros.
        Color y{6};
        auto signal = TrafficLight::red;
        print(++signal == TrafficLight::green);
    }
}
}  // namespace capitulo_2
