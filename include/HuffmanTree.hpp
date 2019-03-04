// Huffman tree abstraction

#ifndef HUFFMAN_TREE_HPP
#define HUFFMAN_TREE_HPP

#include <string>
#include <memory>

#include "Types.hpp"

namespace kpeg
{
    /// Abstraction for a node in a Huffman tree
    struct Node
    {
        Node() :
         root{ false } ,
         leaf{ false } ,
         code{ "" } ,
         value{ 0x00 } ,
         lChild{ nullptr } ,
         rChild{ nullptr } ,
         parent{ nullptr }
        {}
        
        Node(const std::string _code, const UInt16 _val) :
         root{ false } ,
         leaf{ false } ,
         code{ _code } ,
         value{ _val } ,
         lChild{ nullptr } ,
         rChild{ nullptr } ,
         parent{ nullptr }
        {}
        
        // Only the root node is set to true
        bool root;

        // Only the leaves with a symbol value are set to true
        bool leaf;

        // The bitstream binary code (e.g., 1101011...)
        std::string code;

        // The symbol (e.g., 0xC3)
        UInt16 value;

        // The left & right children of the node
        std::shared_ptr<Node> lChild, rChild;

        // Parent of the node, makes it easier to traverse backwards in the tree
        std::shared_ptr<Node> parent;
    };
    
    // Alias for a node
    typedef std::shared_ptr<Node> NodePtr;
    
    /// Node helpers & operations 
    
    /// Create the root node of a Huffman tree
    inline NodePtr createRootNode(const UInt16 value)
    {
        NodePtr root = std::make_shared<Node>( "", value );
        root->root = true;
        return root;
    }
    
    /// Create a node (that is not the root) in a Huffman tree
    inline NodePtr createNode()
    {
        return std::make_shared<Node>();
    }
    
    /// Insert a node as the left child of the specified node
    ///
    /// The value of the child node is specified and a new node
    /// with the same value is created and inserted as the left
    /// child of the specified node
    void insertLeft(NodePtr node, const UInt16 value);
    
    /// Insert a node as the right child of the specified node
    ///
    /// The value of the child node is specified and a new node
    /// with the same value is created and inserted as the right
    /// child of the specified node
    void insertRight(NodePtr node, const UInt16 value);
    
    /// Get the node at the immediate  right of the specified
    /// node, which is also at the same level
    NodePtr getRightLevelNode(NodePtr node);

    /// Perform inorder traversal of the Huffman
    /// tree with specified root node
    void inOrder(NodePtr node);
    
    /// HuffmanTree is an abstraction to manage the binary
    /// tree constructed from the specified Huffman table.
    ///
    /// This class abstracts away the algorithm for
    /// generating the Huffman binary tree for the
    /// Huffman tables found in the JFIF file.    
    class HuffmanTree
    {
        public:
            
            /// Default constructor
            HuffmanTree();
            
            /// Initialize the Huffman tree with specified Huffman table
            ///
            /// @param htable the Huffman table to use
            HuffmanTree(const HuffmanTable& htable);

            /// Create a Huffman tree using specified Huffman table
            ///
            /// @param htable the Huffman table to use
            void constructHuffmanTree(const HuffmanTable& htable);
            
            /// Get the unerlying binray tree that represents the Huffman table
            ///
            /// @return a pointer to the root node of the underlying binary tree
            const NodePtr getTree() const;
            
            /// Check whether a given Huffman code is present in the tree or not
            ///
            // NOTE: std::string is used as the return type because 0x0000 and 0xFFFF
            // are both values that are used in the normal range. So using them is not 
            // possible to indicate special conditions (e.g., code not found in tree)
            const std::string contains(const std::string& huffCode);
            
        private:
            
            // Root of the binary tree
            NodePtr m_root;
    };
}

#endif // HUFFMAN_TREE_HPP
