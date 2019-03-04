// Implementaion of the Huffman tree abstraction

#include <iomanip>

#include "HuffmanTree.hpp"
#include "Utility.hpp"

namespace kpeg
{
    void insertLeft( NodePtr node, const UInt16 value )
    {
        if ( node == nullptr )
            return;
        
        if ( node->lChild != nullptr )
        {
            logFile << "Given node already has a left child, skipping insertion" << std::endl;
            return;
        }
        
        NodePtr lNode = createNode();
        lNode->parent = node;
        node->lChild = lNode;
        
        lNode->code = node->code + "0";
        lNode->value = value;
    }
    
    void insertRight( NodePtr node, const UInt16 value )
    {
        if ( node == nullptr )
            return;
        
        if ( node->rChild != nullptr )
        {
            logFile << "Given node already has a right child, skipping insertion" << std::endl;
            return;
        }
        
        NodePtr rNode = createNode();
        rNode->parent = node;
        node->rChild = rNode;
        
        rNode->code = node->code + "1";
        rNode->value = value;
    }
    
    NodePtr getRightLevelNode( NodePtr node )
    {
        if ( node == nullptr )
            return nullptr;
        
        // Node is the left child of its parent, then the parent's
        // right child is its right level order node.
        if ( node->parent != nullptr && node->parent->lChild == node )
            return node->parent->rChild;
        
        // Else node is the right child of its parent, then traverse
        // back the tree and find its right level order node
        int count = 0;
        NodePtr nptr = node;
        while ( nptr->parent != nullptr && nptr->parent->rChild == nptr )
        {
            nptr = nptr->parent;
            count++;
        }
        
        if ( nptr->parent == nullptr )
            return nullptr;
        
        nptr = nptr->parent->rChild;
        
        int i = 1;
        while ( count > 0 )
        {
            nptr = nptr->lChild;
            count--;
        }
        
        return nptr;        
    }
    
    void inOrder( NodePtr node )
    {
        if ( node == nullptr )
            return;
        inOrder(node->lChild);
        
        if ( node->code != "" && node->leaf )
            logFile << "Symbol: 0x" << std::hex << std::setfill('0') << std::setw(2) << std::setprecision(16) << node->value << ", Code: " << node->code << std::endl;
        
        inOrder(node->rChild);
    }
    
    HuffmanTree::HuffmanTree() :
     m_root{nullptr}
    {
    }
    
    HuffmanTree::HuffmanTree( const HuffmanTable& htable )
    {
        constructHuffmanTree( htable );
    }
    
    void HuffmanTree::constructHuffmanTree( const HuffmanTable& htable )
    {
        logFile << "Constructing Huffman tree with specified Huffman table..." << std::endl;
        
        m_root = createRootNode( 0x0000 );
        insertLeft( m_root, 0x0000 );
        insertRight( m_root, 0x0000 );
        inOrder( m_root );
        NodePtr leftMost = m_root->lChild;
        
        for ( auto i = 1; i <= 16; ++i )
        {
            // If the count is zero, add left & right children for all unassigned leaf nodes.
            if ( htable[i - 1].first == 0 )
            {
                for ( NodePtr nptr = leftMost; nptr != nullptr; nptr = getRightLevelNode( nptr ) )
                {
                    insertLeft( nptr, 0x0000 );
                    insertRight( nptr, 0x0000 );
                }
                
                leftMost = leftMost->lChild;
            }   
            
            // Else assign codes to nodes starting from leftmost leaf in the tree.
            else
            {
                for ( auto&& huffVal : htable[i - 1].second )
                {
                    leftMost->value = huffVal;
                    leftMost->leaf = true;
                    leftMost = getRightLevelNode( leftMost );
                }
                
                insertLeft( leftMost, 0x0000 );
                insertRight( leftMost, 0x0000 );
                
                NodePtr nptr = getRightLevelNode( leftMost );
                leftMost = leftMost->lChild;
                
                while ( nptr != nullptr)
                {
                    insertLeft( nptr, 0x0000 );
                    insertRight( nptr, 0x0000 );
                    
                    nptr = getRightLevelNode( nptr );
                }
            }
        }
        
        logFile << "Finished building Huffman tree [OK]" << std::endl;
    }
    
    const NodePtr HuffmanTree::getTree() const
    {
        return m_root;
    }
    
    const std::string HuffmanTree::contains( const std::string& huffCode )
    {
        if ( utils::isStringWhiteSpace( huffCode ) )
        {
            logFile << "[ FATAL ] Invalid huffman code, possibly corrupt JFIF data stream!" << std::endl;
            return "";
        }
        
        int i = 0;
        NodePtr nptr = m_root;
        
        do
        {
            if ( huffCode[i] == '0' )
                nptr = nptr->lChild;
            else
                nptr = nptr->rChild;
            
            if ( nptr != nullptr && nptr->leaf && nptr->code == huffCode )
            {
                if ( nptr->value == 0x0000 )
                    return "EOB";
                return std::to_string( nptr->value );
            }
            i++;
            
        } while ( nptr != nullptr && i < huffCode.size() );
        
        return "";
    }
}
