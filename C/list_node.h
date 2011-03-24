typedef struct {
    char c;
    void* firstChild;
    void* parent;
    void* sibling;
} Node;

/// returns the child with the given character or null iff there is no such child
inline Node* find_child(Node* parent, char c) {
    parent=parent->firstChild;
    while( parent && parent->c!=c ) {
        parent=parent->sibling;
    }
    return parent;
}


/// returns the sibling with the given character or null iff there is no such child
inline Node* find_sibling(Node* it, char c) {
    while( it && it->c!=c ) {
        it=it->sibling;
    }
    return it;
}

/// returns the sibling with the given character or null iff there is no such child
/**
   \param prev Has to be non-null. Is set to the sibling before the returned one 
 */
inline Node* find_sibling_and_prev(Node* it, char c, Node** prev) {
    while( it && it->c!=c ) {
        *prev=it;
        it=it->sibling;
    }
    return it;
}

void printTreeRec(int depth, Node* root) {
    while(root) {
        int i;
        for(i=0;i<depth;++i) {
            printf("  ");
        }
        if( root->c=='\n' ) {
            printf("EOW\n");
        } else {
            printf("%c\n",root->c);
        }
        printTreeRec(depth+1,root->firstChild);
        root=root->sibling;
    }
}

