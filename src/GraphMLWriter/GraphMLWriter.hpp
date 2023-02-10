#ifndef GRAPHML_WRITER_HPP
#define GRAPHML_WRITER_HPP

#include "Graph/map_graph.hpp"
#include <iostream>
#include <fstream>
#include "util/common.hpp"


/*! \brief Create properties name starting from a type T
 *
 * if T has defined some properties name that name are used otherwise
 * default name are created
 *
 * \tparam T vertex type
 *
 */

template <typename T>
void create_prop(std::string * str)
{
	// if T has attributes defined
	if (has_attributes<T>::value )
	{
		// Create properties names based on the attributes name defined
		for (size_t i = 0 ; i < T::max_prop ; i++)
		{
			str[i] = std::string(T::attributes::name[i]);
		}
	}
	else
	{
		// Create default properties name
		for (size_t i = 0 ; i < T::max_prop ; i++)
		{
			str[i] = "attr" + std::to_string(i);
		}
	}
}

/*! \brief this class is a functor for "for_each" algorithm
 *
 * This class is a functor for "for_each" algorithm. For each
 * element of the boost::vector the operator() is called.
 * Is mainly used to create a string containing all the vertex
 * properties
 *
 */

template<typename G>
struct vertex_prop
{
	//! Properties counter
	int cnt = 0;

	//! vertex properties
	std::string & v_prop;

	//! Attribute names
	std::string * attributes_names;

	//! Number of attributes name defined into the vertex
	int n_attr = 0;

	//! indicate if attributes_names is to destroy
	bool to_destroy = false;

	/*! \brief Constructor
	 *
	 * Create a vertex properties list
	 *
	 * \param v_prop std::string that is filled with the graph properties in the GraphML format
	 * \param a_name array with the names of the properties
	 *
	 */
	vertex_prop(std::string & v_prop, typename G::V_type::attributes & a_name)
	:v_prop(v_prop),attributes_names(a_name.name)
	{
		// Calculate the number of attributes name
		n_attr = sizeof(a_name.name)/sizeof(std::string);
	};

	/*! \brief Constructor
	 *
	 * Create a vertex properties list
	 *
	 * \param v_prop std::string that is filled with the graph properties in the GraphML format
	 *
	 */
	vertex_prop(std::string & v_prop)
	:v_prop(v_prop),attributes_names(NULL)
	{
		// Calculate the number of attributes
		n_attr = G::V_type::max_prop;

		// Create default property names
		attributes_names = new std::string[G::V_type::max_prop];
		to_destroy = true;

		// Create default property names
		create_prop<typename G::V_type>(attributes_names);
	};

	//! destructor
	~vertex_prop()
	{
		if (to_destroy == true)
		{delete [] attributes_names;}
	}

	/*! It call the functor for each member
	 *
	 * \param t each member
	 *
	 */
    template<typename T>
    void operator()(T& t)
    {
    	//! Create an entry for the attribute
    	if (cnt < n_attr)
    	{
    		// if it is a yFile extension property name, does not process it
    		if (attributes_names[cnt] == "x" || attributes_names[cnt] == "y"
    			|| attributes_names[cnt] == "z" || attributes_names[cnt] == "shape" )
    		{cnt++; return ;}

    		// Create a property string based on the type of the property
    		if (std::is_same<T,float>::value)
    			v_prop += "<key id=\"vk" + std::to_string(cnt) + "\" for=\"node\" attr.name=\"" + attributes_names[cnt] + "\" attr.type=\"float\"/>\n";
    		else if (std::is_same<T,double>::value)
    			v_prop += "<key id=\"vk" + std::to_string(cnt) + "\" for=\"node\" attr.name=\"" + attributes_names[cnt] + "\" attr.type=\"double\"/>\n";
    		else if (std::is_same<T,int>::value)
    			v_prop += "<key id=\"vk" + std::to_string(cnt) + "\" for=\"node\" attr.name=\"" + attributes_names[cnt] + "\" attr.type=\"int\"/>\n";
    		else if (std::is_same<T,long int>::value)
    			v_prop += "<key id=\"vk" + std::to_string(cnt) + "\" for=\"node\" attr.name=\"" + attributes_names[cnt] + "\" attr.type=\"long\"/>\n";
    		else if (std::is_same<T,bool>::value)
    			v_prop += "<key id=\"vk" + std::to_string(cnt) + "\" for=\"node\" attr.name=\"" + attributes_names[cnt] + "\" attr.type=\"boolean\"/>\n";
    		else if (std::is_same<T,std::string>::value)
    			v_prop += "<key id=\"vk" + std::to_string(cnt) + "\" for=\"node\" attr.name=\"" + attributes_names[cnt] + "\" attr.type=\"string\"/>\n";
    	}

    	cnt++;
    }
};

/*! \brief this class is a functor for "for_each" algorithm
 *
 * This class is a functor for "for_each" algorithm. For each
 * element of the boost::vector the operator() is called.
 * Is mainly used to create a string containing all the vertex
 * properties
 *
 */

template<typename G>
struct vertex_node
{
	//! Vertex object container
	const typename G::V_container & vo;

	//! Properties counter
	int cnt = 0;

	//! vertex node string
	std::string & v_node;

	//! Attribute names
	std::string * attributes_names;

	//! Number of attributes name defined into the vertex
	int n_attr = 0;

	//! indicate if attributes_names is to destroy
	bool to_destroy = false;

	/*! \brief Constructor
	 *
	 * Create a vertex node
	 *
	 * \param v_node std::string that is filled with the graph node definition in the GraphML format
	 * \param n_obj object container to access its properties for example encapc<...>
	 * \param a_name stub SFINAE, it basically check if G has properties names defined, if yes this
	 *        constructor is selected over the other one
	 *
	 */
	inline vertex_node(std::string & v_node, const typename G::V_container & n_obj, typename G::V_type::attributes & a_name)
	:vo(n_obj),v_node(v_node),attributes_names(a_name.name)
	{
		// Calculate the number of attributes name
		n_attr = sizeof(a_name.name)/sizeof(std::string);
	};

#ifdef DEBUG
	/*! \brief Constructor
	 *
	 * Calling this constructor produce an error. This class store the reference of the object,
	 * this mean that the object passed must not be a temporal object
	 *
	 */
	inline vertex_node(std::string & v_node, const typename G::V_container && n_obj, typename G::V_type::attributes & a_name)
	:vo(n_obj),v_node(v_node),attributes_names(a_name.name)
	{std::cerr << "Error: " <<__FILE__ << ":" << __LINE__ << " Passing a temporal object\n";};
#endif

	/*! \brief Constructor
	 *
	 * Create a vertex properties list
	 *
	 * \param v_node std::string that is filled with the graph properties in the GraphML format
	 * \param n_obj object container to access its properties for example encapc<...>
	 *
	 */
	inline vertex_node(std::string & v_node, const typename G::V_container & n_obj)
	:vo(n_obj),v_node(v_node),attributes_names(NULL)
	{
		// Calculate the number of attributes
		n_attr = G::V_type::max_prop;

		// Create default property names
		attributes_names = new std::string[G::V_type::max_prop];
		to_destroy = true;

		// Create default property names
		create_prop<typename G::V_type>(attributes_names);
	};

	inline ~vertex_node()
	{
		if (to_destroy == true)
		{delete [] attributes_names;}
	}

#ifdef DEBUG
	/*! \brief Constructor
	 *
	 * Calling this constructor produce an error. This class store the reference of the object,
	 * this mean that the object passed must not be a temporal object
	 *
	 */
	inline vertex_node(std::string & v_node, const typename G::V_container && n_obj)
	:vo(n_obj),v_node(v_node),attributes_names(NULL)
	{std::cerr << "Error: " <<__FILE__ << ":" << __LINE__ << " Passing a temporal object\n";};
#endif

	/*! \brief Create a new node
	 *
	 * \param v_c id of the node
	 *
	 */
	void new_node(size_t v_c)
	{
		// start a new node
		v_node += "<node id=\"n"+ std::to_string(v_c) + "\">\n";

		// reset the counter properties
		cnt = 0;
	}

	/*! \brief Close a node
	 *
	 * Close a node
	 *
	 */
	void end_node()
	{
		// close a node
		v_node += "</node>\n";
	}

	/*! \brief It call the functor for each member
	 *
	 * \param t each member
	 *
	 */
    template<typename T>
    void operator()(T& t)
    {
    	//! Create an entry for the attribute
    	if (T::value < n_attr)
    	{
    		typedef typename std::remove_reference<decltype(vo.template get<T::value>())>::type type_get;

    		// Create a property string based on the type of the property
    		if (std::is_same<type_get,float>::value)
    			v_node += "  <data key=\"vk" + std::to_string(cnt) + "\">" + std::to_string(vo.template get<T::value>()) + "</data>\n";
    		else if (std::is_same<type_get,double>::value)
    			v_node += "  <data key=\"vk" + std::to_string(cnt) + "\">" + std::to_string(vo.template get<T::value>()) + "</data>\n";
    		else if (std::is_same<type_get,int>::value )
    			v_node += "  <data key=\"vk" + std::to_string(cnt) + "\">" + std::to_string(vo.template get<T::value>()) + "</data>\n";
    		else if (std::is_same<type_get,long int>::value)
    			v_node += "  <data key=\"vk" + std::to_string(cnt) + "\">" + std::to_string(vo.template get<T::value>()) + "</data>\n";
    		else if (std::is_same<type_get,bool>::value)
    			v_node += "  <data key=\"vk" + std::to_string(cnt) + "\">" + std::to_string(vo.template get<T::value>()) + "</data>\n";
    		else if (std::is_same<type_get,std::string>::value)
    			v_node += "  <data key=\"vk" + std::to_string(cnt) + "\">" + std::to_string(vo.template get<T::value>()) + "</data>\n";
    	}

    	cnt++;
    }
};

/*! \brief this class is a functor for "for_each" algorithm
 *
 * This class is a functor for "for_each" algorithm. For each
 * element of the boost::vector the operator() is called.
 * Is mainly used to create a string containing all the edge
 * properties
 *
 */

template<typename G>
struct edge_prop
{
	//! Properties counter
	int cnt = 0;

	//! edge properties
	std::string & e_prop;

	//! Attribute names
	std::string * attributes_names;

	//! Number of attributes name defined into the vertex
	int n_attr = 0;

	//! indicate if attributes_names is to destroy
	bool to_destroy = false;

	/*! \brief Constructor
	 *
	 * Create an edge properties list
	 *
	 * \param e_prop std::string that is filled with the graph properties in the GraphML format
	 * \param a_name stub SFINAE, it basically check if G::E_type has properties names defined, if yes this
	 *        constructor is selected over the other one
	 *
	 */
	edge_prop(std::string & e_prop, typename G::E_type::attributes & a_name)
	:e_prop(e_prop),attributes_names(a_name.name)
	{
		// Calculate the number of attributes name
		n_attr = sizeof(a_name.name)/sizeof(std::string);
	};

	/*! \brief Constructor
	 *
	 * Create an edge properties list
	 *
	 * \param e_prop std::string that is filled with the graph properties in the GraphML format
	 *
	 */
	edge_prop(std::string & e_prop)
	:e_prop(e_prop),attributes_names(NULL)
	{
		// Calculate the number of attributes
		n_attr = G::E_type::max_prop;

		// Create default property names
		attributes_names = new std::string[G::E_type::max_prop];
		to_destroy = true;

		// Create default property names
		create_prop<typename G::E_type>(attributes_names);
	};

	inline ~edge_prop()
	{
		if (to_destroy == true)
		{delete [] attributes_names;}
	}

	/*! \brief It call the functor for each member
	 *
	 * \param t each member
	 *
	 */
    template<typename T>
    void operator()(T& t)
    {
    	//! Create an entry for the attribute
    	if (cnt < n_attr)
    	{
    		// Create a property string based on the type of the property
    		if (std::is_same<T,float>::value)
    			e_prop += "<key id=\"ek" + std::to_string(cnt) + "\" for=\"edge\" attr.name=\"" + attributes_names[cnt] + "\" attr.type=\"float\"/>\n";
    		else if (std::is_same<T,double>::value)
    			e_prop += "<key id=\"ek" + std::to_string(cnt) + "\" for=\"edge\" attr.name=\"" + attributes_names[cnt] + "\" attr.type=\"double\"/>\n";
    		else if (std::is_same<T,int>::value)
    			e_prop += "<key id=\"ek" + std::to_string(cnt) + "\" for=\"edge\" attr.name=\"" + attributes_names[cnt] + "\" attr.type=\"int\"/>\n";
    		else if (std::is_same<T,long int>::value)
    			e_prop += "<key id=\"ek" + std::to_string(cnt) + "\" for=\"edge\" attr.name=\"" + attributes_names[cnt] + "\" attr.type=\"long\"/>\n";
    		else if (std::is_same<T,bool>::value)
    			e_prop += "<key id=\"ek" + std::to_string(cnt) + "\" for=\"edge\" attr.name=\"" + attributes_names[cnt] + "\" attr.type=\"boolean\"/>\n";
    		else if (std::is_same<T,std::string>::value)
    			e_prop += "<key id=\"ek" + std::to_string(cnt) + "\" for=\"edge\" attr.name=\"" + attributes_names[cnt] + "\" attr.type=\"string\"/>\n";
    	}

    	cnt++;
    }
};

/*! \brief this class is a functor for "for_each" algorithm
 *
 * This class is a functor for "for_each" algorithm. For each
 * element of the boost::vector the operator() is called.
 * Is mainly used to create a string containing all the edge
 * properties
 *
 */

template<typename G>
struct edge_node
{
	//! Vertex object container
	typename G::E_container & vo;

	//! Properties counter
	int cnt = 0;

	//! edge node string
	std::string & e_node;

	//! Attribute names
	std::string * attributes_names;

	//! Number of attributes name defined into the vertex
	int n_attr = 0;

	//! indicate if attributes_names is to destroy
	bool to_destroy = false;

	/*! \brief Constructor
	 *
	 * Create an edge node
	 *
	 * \param e_node std::string that is filled with the graph node definition in the GraphML format
	 * \param n_obj object container to access the object properties for example encapc<...>
	 * \param a_name attributes names
	 *
	 */
	edge_node(std::string & e_node, typename G::E_container & n_obj, typename G::E_type::attributes & a_name)
	:vo(n_obj),e_node(e_node),attributes_names(a_name.name)
	{
		// Calculate the number of attributes name
		n_attr = sizeof(a_name.name)/sizeof(std::string);
	};

	/*! \brief Constructor
	 *
	 * Create an edge node
	 *
	 * \param e_node std::string that is filled with the graph properties in the GraphML format
	 * \param n_obj object container to access the object properties for example encapc<...>
	 *
	 */
	edge_node(std::string & e_node, typename G::E_container & n_obj)
	:vo(n_obj),e_node(e_node),attributes_names(NULL)
	{
		// Calculate the number of attributes
		n_attr = G::E_type::max_prop;

		// Create a number of default properties name
		attributes_names  = new std::string[G::E_type::max_prop];
		to_destroy = true;

		// Create default property names
		create_prop<typename G::E_type>(attributes_names);

	};

	inline ~edge_node()
	{
		if (to_destroy == true)
		{delete [] attributes_names;}
	}

	/*! \brief Create a new node
	 *
	 * \param v_c node number
	 * \param s source node id
	 * \param d destination node id
	 *
	 */
	void new_node(size_t v_c, size_t s, size_t d)
	{
		// start a new node
		e_node += "<edge id=\"e"+ std::to_string(v_c) + "\" source=\"n" + std::to_string(s) + "\" target=\"n" + std::to_string(d) + "\">\n";

		// reset the counter properties
		cnt = 0;
	}

	/*! \brief Close a node
	 *
	 * Close a node
	 *
	 */
	void end_node()
	{
		// close a node
		e_node += "</edge>\n";
	}

	/*! It call the functor for each member
	 *
	 * \param t each member
	 *
	 */
    template<typename T>
    void operator()(T& t)
    {
    	//! Create an entry for the attribute
    	if (T::value < n_attr)
    	{
    		typedef typename std::remove_reference<decltype(vo.template get<T::value>())>::type type_get;

    		// Create a property string based on the type of the property
    		if (std::is_same<type_get,float>::value)
    			e_node += "  <data key=\"ek" + std::to_string(cnt) + "\">" + std::to_string(vo.template get<T::value>()) + "</data>\n";
    		else if (std::is_same<type_get,double>::value)
    			e_node += "  <data key=\"ek" + std::to_string(cnt) + "\">" + std::to_string(vo.template get<T::value>()) + "</data>\n";
    		else if (std::is_same<type_get,int>::value)
    			e_node += "  <data key=\"ek" + std::to_string(cnt) + "\">" + std::to_string(vo.template get<T::value>()) + "</data>\n";
    		else if (std::is_same<type_get,long int>::value)
    			e_node += "  <data key=\"ek" + std::to_string(cnt) + "\">" + std::to_string(vo.template get<T::value>()) + "</data>\n";
    		else if (std::is_same<type_get,bool>::value)
    			e_node += "  <data key=\"ek" + std::to_string(cnt) + "\">" + std::to_string(vo.template get<T::value>()) + "</data>\n";
    		else if (std::is_same<type_get,std::string>::value)
    			e_node += "  <data key=\"ek" + std::to_string(cnt) + "\">" + std::to_string(vo.template get<T::value>()) + "</data>\n";
    	}

    	cnt++;
    }
};

/*!
 *
 * From a Graphbasic structure it write a GraphML format file
 *
 */

template <typename Graph>
class GraphMLWriter
{
	//! Graph to write
	Graph & g;

	/*! \brief It get the vertex properties list
	 *
	 * It get the vertex properties list of the vertex defined as a GraphML header
	 * and
	 * define position and shape of the node
	 *
	 * \return a string that define the vertex properties in graphML format
	 *
	 */
	std::string get_vertex_properties_list()
	{
		//! vertex property output string
		std::string v_out("");

		// create a vertex property functor
		vertex_prop<Graph> vp(v_out);

		// Iterate through all the vertex and create the vertex list
		boost::mpl::for_each_ref< typename Graph::V_type::type >(vp);

		// return the vertex properties string
		return v_out;
	}

	/*! \brief It get the edge properties list
	 *
	 * It get the edge properties list of the edge defined as a GraphML header
	 *
	 * \return a string that define the edge properties in graphML format
	 *
	 */
	std::string get_edge_properties_list()
	{
		//! edge property output string
		std::string e_out;

		// create a vertex property functor
		edge_prop<Graph> ep(e_out);

		// Iterate through all the vertex and create the vertex list
		boost::mpl::for_each_ref< typename Graph::E_type::type >(ep);

		// return the edge properties string
		return e_out;
	}

	/*! \brief Get the string containing the set of vertices
	 *
	 * \return the set of vertices as string
	 *
	 */
	std::string get_vertex_list()
	{
		// node counter
		size_t nc = 0;

		//! vertex node output string
		std::string v_out;

		//! Get a vertex iterator
		auto it = g.getVertexIterator();

		// if there is the next element
		while (it.isNext())
		{
			auto v = g.vertex(it.get());

			// create a vertex list functor
			vertex_node<Graph> vn(v_out,v);

			// create new node
			vn.new_node(nc);

			// Iterate through all the vertex and create the vertex list
			boost::mpl::for_each_ref< boost::mpl::range_c<int,0,Graph::V_type::max_prop> >(vn);

			// end node
			vn.end_node();

			// increment the iterator and counter
			++it;
			nc++;
		}

		// return the vertex list
		return v_out;
	}

	/*! \brief return the edge list as a string
	 *
	 * \return the edge list as string
	 *
	 */
	std::string get_edge_list()
	{
		// node counter
		size_t nc = 0;

		//! edge node output string
		std::string e_out;

		//! Get an edge iterator
		auto it = g.getEdgeIterator();

		// if there is the next element
		while (it.isNext())
		{
			// Get the edge object
			auto obj = g.edge(it.get());

			// create an edge list functor
			edge_node<Graph> en(e_out,obj);

			// create a new node
			en.new_node(nc,it.source(),it.target());

			// Iterate through all the edges and create the edge list
			boost::mpl::for_each_ref< boost::mpl::range_c<int,0,Graph::E_type::max_prop> >(en);

			// end new node
			en.end_node();

			// increment the operator
			++it;
			nc++;
		}

		// return the edge list
		return e_out;
	}

public:

	/*!
	 *
	 * GraphMLWriter constructor, it take a graph and write a GraphML format
	 *
	 * \param g Graph to write
	 *
	 */
	GraphMLWriter(Graph & g)
	:g(g)
	{}

	/*! \brief It write a GraphML file from a graph
	 *
	 * \param file path where to write
	 * \param name of the graph
	 *
	 */
	bool write(std::string file, std::string graph_name="Graph")
	{
		// Header for the GraphML
		std::string gml_header;
		// Vertex list of the GraphML
		std::string vertex_list;
		// End for the GraphML
		std::string gml_header_end;
		// Graph header
		std::string graph_header;
		// Graph header end
		std::string graph_header_end;
		// Edge list of the GraphML
		std::string edge_list;
		// vertex properties header
		std::string vertex_prop_header;
		// edge properties header
		std::string edge_prop_header;

		// GraphML header
		gml_header = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
		<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\"\n\
		    xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n\
		    xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns\n\
		     http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n";

		// Graph header to define an header
		graph_header = "<graph id=\"" + graph_name + "\" edgedefault=\"undirected\">\n";
		// Graph header end
		graph_header_end =  "</graph>\n";

		// Vertex properties header
		vertex_prop_header = get_vertex_properties_list();

		// Edge properties header
		edge_prop_header = get_edge_properties_list();

		// Get the node graph list
		vertex_list = get_vertex_list();

		// Get the edge graph list
		edge_list = get_edge_list();

		// Header end
		gml_header_end = "</graphml>";

		// write the file

		std::ofstream ofs(file);

		// Check if the file is open
		if (ofs.is_open() == false)
		{std::cerr << "Error cannot creare the graphML file: " + file;}

		ofs << gml_header << graph_header << vertex_prop_header << edge_prop_header <<
			   vertex_list << edge_list << graph_header_end << gml_header_end;

		// Close the file

		ofs.close();

		// Completed succefully
		return true;
	}
};

#endif
