struct NullType;
struct EmptyList;

template <typename T, typename ...Ts>
struct TypeList
{
  using Head = T;
  using Tail = TypeList<Ts...>;
};

template <typename T>
struct TypeList <T>
{
  using Head = T;
  using Tail = EmptyList;
};

template <typename TL, size_t ind>
struct TypeAt : TypeAt <typename TL::Tail, ind - 1> {};

template <size_t ind>
struct TypeAt <EmptyList, ind>
{
  using res = NullType;
};

template <typename TL>
struct TypeAt <TL, 0>
{
  using res = typename TL::Head;
};

template <>
struct TypeAt <EmptyList, 0>
{
  using res = NullType;
};

// get number of cells in layer
template <size_t layer>
struct LayerLen
{
  static const size_t val = 3*LayerLen<layer - 2>::val;
};

template <>
struct LayerLen <1>
{
  static const size_t val = 3;
};

template <>
struct LayerLen <2>
{
  static const size_t val = 3;
};

// get indexes of the most left and right cells in a layer
template <size_t layer>
struct LayerBounds
{
  static const size_t left = LayerBounds <layer - 1>::right + 1;
  static const size_t right = left + LayerLen <layer>::val - 1;
};

template <>
struct LayerBounds <1>
{
  static const size_t left = 1;
  static const size_t right = 3;
};

// get the number of base classes
template <size_t layer>
struct Step
{
  static const size_t val = (Step <layer - 1>::val + 2) % 4;
};

template <>
struct Step <1>
{
  static const size_t val = 1;
};

// get position of the most left base cell in the next layer
template <size_t ind, size_t layer>
struct GetIndInNextLayer
{
  static const size_t res = GetIndInNextLayer <ind - 1, layer>::res + Step <layer>::val;
};

template <size_t layer>
struct GetIndInNextLayer <0, layer>
{
  static const size_t res = 0;
};

// get index of the most left base cell in the next layer
template <size_t ind, size_t layer>
struct GetInd
{
  static const size_t res = LayerBounds <layer + 1>::left + 
      GetIndInNextLayer <ind - LayerBounds <layer>::left, layer>::res;
};

/*
 * U - Unit class
 * T - Unit template
 * layer - number of the current layer
 * b - binary which shows how many base classes should we create (0 --> 1, 1 --> 3)
 * ind - index of the cell
 */
template <template <typename> typename U, typename T, typename TL, size_t layer, bool b, size_t ind>
struct Cell;

template <template <typename> typename U, typename TL, size_t layer, bool b, size_t ind>
struct Cell <U, NullType, TL, layer, b, ind> {};

template <template <typename> typename U, typename TL, size_t layer, size_t ind>
struct Cell <U, NullType, TL, layer, 0, ind> {};

template <template <typename> typename U, typename TL, size_t layer, size_t ind>
struct Cell <U, NullType, TL, layer, 1, ind> {};

template <template <typename> typename U, typename T, typename TL, size_t layer, size_t ind>
struct Cell <U, T, TL, layer, 0, ind> : Cell <U, typename TypeAt <TL, GetInd <ind, layer>::res>::res, 
    TL, layer + 1, 1, GetInd <ind, layer>::res>
{
  U <T> unit;
};

template <template <typename> typename U, typename T, typename TL, size_t layer, size_t ind>
struct Cell <U, T, TL, layer, 1, ind> :
    Cell <U, typename TypeAt <TL, GetInd <ind, layer>::res>::res, TL,layer + 1,0, GetInd <ind, layer>::res>,
    Cell <U, typename TypeAt <TL, GetInd <ind, layer>::res + 1>::res, TL, layer + 1, 0, GetInd <ind, layer>::res + 1>,
    Cell <U, typename TypeAt <TL, GetInd <ind, layer>::res + 2>::res, TL, layer + 1, 0, GetInd <ind, layer>::res + 2>
{
  U <T> unit;
};

template <template <typename> typename U, typename TL>
struct GenCustomHierarchy :
    Cell <U, typename TypeAt <TL, 1>::res, TL, 1, 0, 1>,
    Cell <U, typename TypeAt <TL, 2>::res, TL, 1, 0, 2>,
    Cell <U, typename TypeAt <TL, 3>::res, TL, 1, 0, 3>
{
  U <TypeAt <TL, 0> > unit;
};

template <typename T>
struct Unit
{
  T x;
};

int main()
{
  GenCustomHierarchy <Unit, TypeList <int, char, int, int, int, int, double, float, char>> gen;
  return 0;
}
