#ifndef GRAPH__GRAPH_H_
#define GRAPH__GRAPH_H_

template <typename V>
class GraphI
{
 protected:
  size_t _vertices_num = 0;
  size_t _edges_num = 0;

 public:
  size_t vertices_num() const noexcept
  {
    return _vertices_num;
  }
  virtual size_t edges_num() const noexcept
  {
    return _edges_num;
  }
  virtual void add_vertex(V) = 0;
  virtual void add_edge(V, V) = 0;
  virtual bool edge_exists(const V&, const V&) const noexcept = 0;
  virtual bool vertex_exists(const V&) const noexcept = 0;
};

template <typename V, int type = 0>
class DirectedGraph;

template <typename V>
class DirectedGraph <V, 0> : public GraphI <V>
{
 private:
  std::unordered_map <V&, std::unordered_map<V, bool> > _adj_matrix;

 public:
  void add_vertex(V vertex) override
  {
    if(!vertex_exists(vertex))
    {
      _adj_matrix[vertex] = std::unordered_map<V, bool>();
      ++GraphI<V>::_vertices_num;
    }
  }

  virtual void add_edge(V vertex1, V vertex2) override
  {
    if(!edge_exists(vertex1, vertex2))
    {
      _adj_matrix[vertex1][vertex2] = 1;
      ++GraphI<V>::_edges_num;
    }
  }

  bool edge_exists(const V &vertex1, const V &vertex2) const noexcept override
  {
    if(_adj_matrix.count(vertex1) && _adj_matrix.at(vertex1).count(vertex2))
    {
      return _adj_matrix.at(vertex1).at(vertex2);
    }
    return false;
  }

  bool vertex_exists(const V &vertex) const noexcept override
  {
    return _adj_matrix.count(vertex);
  }
};

template <typename V>
class DirectedGraph <V, 1> : public GraphI<V>
{
 private:
  std::unordered_map<V, std::unordered_set<V> > mp_vertex_neighbours;

 public:
  void add_vertex(V vertex) override
  {
    if(!vertex_exists(vertex))
    {
      mp_vertex_neighbours[vertex] = {};
      ++GraphI<V>::_vertices_num;
    }
  }

  bool edge_exists(const V &vertex1, const V &vertex2) const noexcept override
  {
    return mp_vertex_neighbours.count(vertex1) && mp_vertex_neighbours.at(vertex1).count(vertex2);
  }

  virtual void add_edge(V vertex1, V vertex2) override
  {
    if(!edge_exists(vertex1, vertex2))
    {
      mp_vertex_neighbours[vertex1].insert(vertex2);
      ++GraphI<V>::_edges_num;
    }
  }

  bool vertex_exists(const V &vertex) const noexcept override
  {
    return mp_vertex_neighbours.count(vertex);
  }
};

template <typename V, int type = 0>
class Graph : public DirectedGraph <V, type>
{
  virtual void add_edge(V vertex1, V vertex2)
  {
    DirectedGraph<V, type>::add_edge(vertex1, vertex2);
    DirectedGraph<V, type>::add_edge(vertex2, vertex1);
  }

  size_t edges_num() const noexcept
  {
    return GraphI<V>::_edges_num >> 1;
  }
};

template <typename V, typename W, int type = 0>
class WeightedDirectedGraph : public DirectedGraph <V, type>
{
 private:
  std::unordered_map <V, std::unordered_map <V, W> > weights_matrix;

 public:
  W get_edge_weight(V vertex1, V vertex2)
  {
    return weights_matrix.at(vertex1).at(vertex2);
  }

  virtual void add_edge(V vertex1, V vertex2, W weight)
  {
    DirectedGraph <V, type>::add_edge(vertex1, vertex2);
    weights_matrix[vertex1][vertex2] = weight;
  }
};

template <typename V, typename W, int type = 0>
class WeightedGraph: public WeightedDirectedGraph <V, W, type>
{
 private:
  std::unordered_map <V, std::unordered_map <V, W> > weights_matrix;

 public:
  size_t edges_num() const noexcept override
  {
    return GraphI<V>::_edges_num >> 1;
  }

  void add_edge(V vertex1, V vertex2, W weight) override
  {
    WeightedDirectedGraph<V, W, type>::add_edge(vertex1, vertex2, weight);
    WeightedDirectedGraph<V, W, type>::add_edge(vertex2, vertex1, weight);
  }
};

struct EmptyType {};

template <bool h, typename t>
struct Matrix2D
{
  static const bool head = h;
  using tail = t;
};

template <bool h, typename M>
struct PushFront
{
  using res = Matrix2D <h, M>;
};

template <size_t w, size_t h, size_t i, size_t j>
struct FindPos
{
  static const size_t pos = w*i + j;
};

template <typename M, size_t i>
struct GetAt
{
  static const bool val = GetAt<typename M::tail, i - 1>::val;
};

template <typename M>
struct GetAt<M, 0>
{
  static const bool val = M::head;
};

template <typename M, size_t i, bool b>
struct SetAt
{
  using res = Matrix2D<M::head, typename SetAt <typename M::tail, i - 1 , b>::res>;
};

template <typename M, bool b>
struct SetAt <M, 0, b>
{
  using res = Matrix2D<b, typename M::tail>;
};

template <size_t N>
struct ZeroMatrix
{
  using res = typename PushFront<0, typename ZeroMatrix<N - 1>::res>::res;
};

template <>
struct  ZeroMatrix<1>
{
  using res = Matrix2D <0, EmptyType>;
};

template <size_t N>
struct ZeroSquaredMatrix
{
  using res = typename ZeroMatrix <N*N>::res;
};

template <size_t n, typename M = EmptyType, size_t e = 0>
struct CompileTimeGraph
{
  static const size_t N = n;
  static const size_t edges_num = e;
  using AdjMatrix = M;
};

template <size_t n>
struct CompileTimeGraph <n, EmptyType, 0>
{
  static const size_t N = n;
  static const size_t edges_num = 0;
  using AdjMatrix = typename ZeroSquaredMatrix <n>::res;
};

template <typename G, size_t i, size_t j>
struct AddEdge
{
  using M1 = typename SetAt <typename G::AdjMatrix, FindPos <G::N, G::N, i, j>::pos, 1>::res;
  using M2 = typename SetAt <typename G::AdjMatrix, FindPos <G::N, G::N, j, i>::pos, 1>::res;
  using res = CompileTimeGraph<G::N, M2, G::edges_num + 1 -
  GetAt<typename G::AdjMatrix, FindPos<G::N, G::N, i, j>::pos>::val >;
};

template <typename G, size_t i, size_t j>
struct EdgeExists
{
  static const bool res = GetAt<typename G::AdjMatrix, FindPos<G::N, G::N, i, j>::pos>::val;
};

#endif //GRAPH__GRAPH_H_
