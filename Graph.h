
#ifndef __GRAPH_H__
#define __GRAPH_H__

#pragma once

#include <iostream>
using namespace std;

#include <vector>
#include <queue>
#include <map>
#include <assert.h>
#include <D:\Github\STL\UnionFindSet.h>

//�ڽӾ����ʾ���ռ��˷Ѵ�
template <class V, class W, bool digraph = false>
class GraphMartix
{
public:
	GraphMartix(const V *vertex, size_t n)
	{
		_vertex.resize(n, V());
		for (size_t i = 0; i < n; ++i){
			_vertex[i] = vertex[i];
			_index[_vertex[i]] = i;
		}

		_martixEdge = new W*[n];
		for (size_t i = 0; i < n; ++i){
			_martixEdge[i] = new W[n];
			memset(_martixEdge[i], W(), sizeof(W)*n);
		}
	}

	void AddEdge(const V& src, const V& dst, const W& w)
	{
		size_t srcIndex = _GetIndex(src);
		size_t dstIndex = _GetIndex(dst);

		_martixEdge[srcIndex][dstIndex] = w;
		if (false == digraph)
			_martixEdge[dstIndex][srcIndex] = w;
	}
	W& GetLink(const V& src, const V& dst)
	{
		size_t srcIndex = _GetIndex(src);
		size_t dstIndex = _GetIndex(dst);

		return _martixEdge[srcIndex][dstIndex];
	}
private:
	size_t _GetIndex(const V& v)
	{
		assert(1 == _index.count(v));
		return _index[v];
	}
private:
	map<V, size_t> _index;//ͨ���������±�
	vector<V> _vertex;//ͨ���±��Ҷ���
	W** _martixEdge;//��ά����
};

template<class V, class W>
struct __LinkNode
{
	__LinkNode()
	:_src(-1)
	, _dst(-1)
	, _w(W())
	, _next(NULL)
	{}

	__LinkNode(size_t src, size_t dst, const W& w)
		:_src(src)
		, _dst(dst)
		, _w(w)
		, _next(NULL)
	{}
	size_t _src;
	size_t _dst;
	W _w;
	__LinkNode* _next;
};

//�ڽӱ�
template <class V, class W, bool digraph = false>//false ��ʾ������ͼ
class GraphLink
{
	typedef __LinkNode<V, W> Link;
public:
	GraphLink(const V *vertex, size_t n)
		:_edgeSize(0)
	{
		_vertex.resize(n, V());
		_linkEdge.resize(n, NULL);
		for (size_t i = 0; i < n; ++i){
			_vertex[i] = vertex[i];
			_index[_vertex[i]] = i;
		}

	}

	//���ӱ�
	void AddEdge(const V& src, const V& dst, const W& w)
	{
		size_t srcIndex = _GetIndex(src);
		size_t dstIndex = _GetIndex(dst);
		
		_AddEdge(srcIndex, dstIndex, w);
	}
	//��ȡ�ߵ�Ȩֵ
	Link* GetLink(const V& src, const V& dst)
	{
		size_t srcIndex = _GetIndex(src);
		size_t dstIndex = _GetIndex(dst);

		Link* edge = _linkEdge[srcIndex];
		while (edge){
			if (edge->_dst == dstIndex)
				return edge;
			edge = edge->_next;
		}

		Link invalid;
		return &invalid;
		//return NULL;
	}

	//������ȱ���
	void GFS(const V& start)
	{
		assert(false == digraph);
		cout << "GFS: ";
		size_t startIndex = _GetIndex(start);

		vector<bool> visited;
		visited.resize(_vertex.size(), false);

		queue<size_t> q;
		q.push(startIndex);

		while (!q.empty()){
			size_t index = q.front();
			q.pop();
			if (false == visited[index]){
				cout << index << ':' << _vertex[index].c_str() << "-> ";
				visited[index] = true;
			}

			Link* edge = _linkEdge[index];
			while (edge){
				if (false == visited[edge->_dst])
					q.push(edge->_dst);
				edge = edge->_next;
			}
		}
		cout << "NULL" << endl;
	}

	//������ȱ���
	void DFS(const V& start)
	{
		assert(false == digraph);
		cout << "DFS: ";
		size_t index = _GetIndex(start);
		vector<bool> visited;
		visited.resize(_vertex.size(), false);
		_DFS(index, visited);
		cout << "NULL" << endl;
	}

	//��С������
	bool MinTree(GraphLink& gp)
	{
		return _Kruskal(gp);
		//return _Prim(gp);
		//return _Dijkstra(gp);
	}

private:
	void _AddEdge(size_t srcIndex, size_t dstIndex, W w)
	{
		Link* newEdge = new Link(srcIndex, dstIndex, w);
		newEdge->_next = _linkEdge[srcIndex];
		_linkEdge[srcIndex] = newEdge;
		++_edgeSize;

		if (false == digraph){
			newEdge = new Link(dstIndex, srcIndex, w);
			newEdge->_next = _linkEdge[dstIndex];
			_linkEdge[dstIndex] = newEdge;
			++_edgeSize;
		}
	}

	//Kruscal
	bool _Kruskal(GraphLink& gp)
	{
		/*
		�ò��鼯
		1. �Ƚ����б߷ŵ�һ�������У�������
		2. �����ж���ŵ�һ�����鼯�У��տ�ʼ����Ϊһ����������
		3. ���δ�������ȡ��һ���ߣ�����ߵ��������� �� ��ͬһ�������У�
			������������ӵ���С�������У��������ߵ���������ϲ���ͬһ��������
		4. ����ߵĸ������ڶ������-1�������
		*/

		//O(N)
		vector<Link*> edges;
		edges.reserve(_linkEdge.size());
		for (size_t i = 0; i < _linkEdge.size(); ++i){
			Link *edge = _linkEdge[i];
			while (edge){
				if(edge->_src < edge->_dst)
					edges.push_back(edge);
				edge = edge->_next;
			}
		}

		struct Less{
			bool operator()(const Link* l, const Link* r){
				return l->_w < r->_w;
			}
		};

		//O(NlogN)
		std::sort(edges.begin(), edges.end(), Less());//����ıߵ�����
		
		UnionFindSet ufs(edges.size());
		int n = _vertex.size()-1;

		for (size_t i = 0; i < edges.size(); ++i){
			if (ufs.InOneSet(edges[i]->_src, edges[i]->_dst))
				continue;

			gp._AddEdge(edges[i]->_src, edges[i]->_dst, edges[i]->_w);//���ӱ�
			ufs.Union(edges[i]->_src, edges[i]->_dst);//�ϲ�����
			if (--n == 0)
				return true;
		}

		return false;
	}
	//Prim
	bool _Prim(GraphLink& gp)
	{
		/*
		�����ж���ֳ���������
		*/
		vector<bool> div;//�����Ƿ���ͬһ��������
		div.resize(_index.size(), false);

		struct Greater{
			bool operator()(const Link* l, const Link* r){
				return l->_w > r->_w;
			}
		};

		priority_queue<Link*, vector<Link*>, Greater> pq;
		div[0] = true;

		Link *edge = _linkEdge[0];
		while (edge){
			pq.push(edge);
			edge = edge->_next;
		}

		int n = _vertex.size() - 1;
		while (!pq.empty()){
			edge = pq.top();//ÿ��ȡ��̵ı�
			if (false == div[edge->_dst]){
				//gp._AddEdge(edge->_src, edge->_dst, edge->_w);
				gp.AddEdge(_vertex[edge->_src], _vertex[edge->_dst], edge->_w);
				div[edge->_dst] = true;
				if (--n == 0)
					return true;
			}
			pq.pop();

			edge = _linkEdge[edge->_dst];
			while (edge){
				if(false == div[edge->_dst])
					pq.push(edge);
				edge = edge->_next;
			}
		}
		return false;
	}

	//Dijkstra
	void _Dijkstra(const GraphLink& gp)
	{
		vector<W> dist;
		vector<Link*> path;
		dist.resize(_vertex.size(), -1);
		path.resize(_vertex.size(), NULL);

		for (size_t i = 0; i < _linkEdge.size(); ++i){
			Link* edge = _linkEdge[0];
			while (edge){
				if (dist[edge->_dst] == -1 || dist[edge->_dst] > edge->_w)
					dist[edge->_dst] = edge->_w;
			}
		}
	
	}

	//�������
	void _DFS(size_t index, vector<bool>& visited)
	{
		cout << index << ':' << _vertex[index].c_str() << "-> ";
		visited[index] = true;

		Link* edge = _linkEdge[index];
		while (edge){
			if(false == visited[edge->_dst])
				_DFS(edge->_dst, visited);
			edge = edge->_next;
		}
	}

	size_t _GetIndex(const V& v)
	{
		assert(1 == _index.count(v));
		return _index[v];
	}
private:
	map<V, size_t> _index;//ͨ���������±�
	vector<V> _vertex;//ͨ���±��Ҷ���
	vector<Link*> _linkEdge;//�ڽӱ�
	size_t _edgeSize;//�ж�������
};

#endif

#if 1
//����

//void TestGraphMartic()
//{
//	const int N = 6;
//	string strs[N] = { "����", "����", "����", "����", "�Ӱ�", "����" };
//	GraphMartix<string, int> m(strs, N);
//
//	m.AddEdge("����", "����", 50);
//	m.AddEdge("����", "�Ӱ�", 200);
//	m.AddEdge("����", "����", 100);
//	m.AddEdge("����", "����", 180);
//	m.AddEdge("����", "����", 120);
//
//	cout << m.GetLink("����", "����") << endl;
//	cout << m.GetLink("����", "�Ӱ�") << endl;
//	cout << m.GetLink("����", "����") << endl;
//	cout << m.GetLink("����", "����") << endl;
//
//}

void TestGraphLink()
{
	const int N = 6;
	string strs[N] = { "����", "����", "����", "����", "�Ӱ�", "����" };
	GraphLink<string, int> gl(strs, N);

	gl.AddEdge("����", "�Ӱ�", 200);
	gl.AddEdge("����", "����", 100);
	gl.AddEdge("����", "����", 50);
	gl.AddEdge("����", "����", 120);
	gl.AddEdge("����", "����", 140);
	gl.AddEdge("����", "����", 180);


	GraphLink<string, int> minTree(strs, N);
	gl.MinTree(minTree);

	//cout << m.GetLink("����", "����")->_w << endl;
	//cout << m.GetLink("����", "�Ӱ�")->_w << endl;
	//cout << m.GetLink("����", "����")->_w << endl;
	//cout << m.GetLink("����", "����")->_w << endl;

	//m.GFS("����");
	//m.DFS("����");
}

#endif