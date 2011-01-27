#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char c;
    void* firstChild;
    void* parent;
    void* sibling;
} Node;

Node* find_child(Node* parent, char c) {
    parent=parent->firstChild;
    while( parent && parent->c!=c ) {
        parent=parent->sibling;
    }
    return parent;
}

typedef struct {
    char** matrix;
    int width;
    int height;
} Matrix;


#define WORD_CHUNK 8

void printTreeRec(int depth, Node* node) {
    while(node) {
        int i;
        for(i=0;i<depth;++i) {
            printf("  ");
        }
        if( node->c=='\n' ) {
            printf("EOW\n");
        } else {
            printf("%c\n",node->c);
        }
        printTreeRec(depth+1,node->firstChild);
        node=node->sibling;
    }
}

Matrix* parseMatrix(const char* matrixFileName) {
    Matrix* matrix=malloc( sizeof( Matrix ));
    int i=0;
    int j=0;
    matrix->width=0;
    matrix->height=0;
    matrix->matrix=malloc( 0 );

    FILE* file = fopen(matrixFileName,"r");
    char buf[255];
    while( fgets(buf,255,file) ) {
        if( buf[0]=='\n' ) {
            break;
        }
        i=matrix->height;
        matrix->matrix=realloc( matrix->matrix, (i+1)*sizeof(char*));
        int l=strlen(buf);
        if( matrix->width==0 ) {
            matrix->width=l-1;
        } else {
            if( matrix->width!=l-1 ) {
                printf("Not all lines in matrix have the same number of characters!\n");
            }
        }
        matrix->matrix[i]=malloc( l );
        strncpy(matrix->matrix[i],buf,l);
        matrix->matrix[i][l-1]=0;
        ++matrix->height;
    }
    printf("read %dx%d matrix:\n",matrix->height,matrix->width);
    for( i=0;i<matrix->height;++i ) {
        for( j=0;j<matrix->width;++j ) {
            printf("%c ",matrix->matrix[i][j]);
        }
        printf(" |\n");
    }
    return matrix;
    
}

Node* dict2tree(const char* dictFileName) {

    FILE* dict = fopen(dictFileName,"r");

    char buf[256];

    Node* root=malloc( sizeof( Node ) );

    root->c='_';
    root->parent=NULL;
    root->sibling=NULL;

    Node* curParent=0;
    Node* prev=root;
    Node* it=root;
    int i;

    while( fgets(buf,255,dict) ) {
//        printf("entering '%s'\n",buf);
        i=0;
        it=root;
        int l=strlen(buf);
        while( it ) {
            while( it && it->c!=buf[i] ) {
                prev=it;
                it=it->sibling;
            }
            if( !it ) {
                prev->sibling=malloc( sizeof( Node ) );
                it=prev->sibling;
                it->c=buf[i];
                it->parent=curParent;
                it->sibling=NULL;
                it->firstChild=NULL;
            }
            ++i;
            curParent=it;
            if( i>=l ) {
                break;
            }
            it=it->firstChild;
        }
        if( !it ) {
            it=curParent;
            for(;i<l;++i) {
                it->firstChild=malloc( sizeof( Node ) );
                curParent=it;
                it=it->firstChild;
                it->c=buf[i];
                it->parent=curParent;
                it->sibling=NULL;
                it->firstChild=NULL;
            }
        }
    }

    prev=root;
    root=root->sibling;
    free(prev);

    return root;
}

int word_in_tree(Node* root, const char* word) {
    int l=strlen(word);
    int i=0;
    while( root ) {
        while( root && root->c!=word[i] ) {
            root=root->sibling;
        }
        if( !root ) {
            break;
        }
        ++i;
        root=root->firstChild;
        if( i>=l ) {
            break;
        }
    }
    if( root ) {
        while( root && root->c!='\n' ) {
            root=root->sibling;
        }
        if( root ) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

typedef struct {
    int row;
    int col;
    char* word;
    int length;
    Node* node;
    void* next;
} Frame;

int wordsInMatrix(Node* root, Matrix* matrix) {
    int i;
    int j;
    char c;
    Frame* last=NULL;
    Frame* first=NULL;
    Node* child=NULL;
    Frame* frame=NULL;
    Node* sibling=NULL;
    for(i=0;i<matrix->width;++i) {
        for(j=0;j<matrix->height;++j) {

            c=matrix->matrix[j][i];
            sibling=root;
            while( sibling && sibling->c!=c ) {
                sibling=sibling->sibling;
            }
            if( sibling ) {
                frame=malloc(sizeof(Frame));
//                printf("initial node %p (%d,%d) c=%c\n",frame,j,i,c);
                frame->row=j;
                frame->col=i;
                frame->word=calloc(2,sizeof(char));
                frame->word[0]=c;
                //                frame->word[1]=0;
                frame->length=1;
                frame->node=sibling;

                if( last ) {
                    last->next=frame;
                } else {
                    first=frame;
                }
                last=frame;
            }
        }
    }
    if( last ) {
        last->next=NULL;
    }
    frame=first;
    int row,col;
    int steps[4][2]={{-1,0},{1,0},{0,-1},{0,1}};
    while( first ) {
        //        c=first->word[first->length-1];
        c=first->node->c;
//        printf("node %p with c=%c, length %d, (%d,%d)\n",first,first->node->c,first->length,first->row,first->col);

        if( (child=find_child(first->node, '\n')) ) {
            printf("found word '%s'\n",first->word);
        }
/*         if( strcmp(first->word,"eat")==0 ) { */
/*             printf("@ eat!\n"); */
/*         } */
/*         if( strcmp(first->word,"eata")==0 ) { */
/*             printf("@ eata!\n"); */
/*         } */
        for(i=0;i<4;++i) {
            row=first->row+steps[i][0];
            col=first->col+steps[i][1];
        
            if( row>=0 && row<matrix->height && col>=0 && col<matrix->width ) {
                c=matrix->matrix[row][col];
                child=find_child(first->node, c);
                if( child ) {
                    frame=malloc(sizeof(Frame));
//                    printf("creating node %p\n",frame);
                    frame->row=row;
                    frame->col=col;
                    frame->length=first->length+1;
                    frame->word=calloc((frame->length+1),sizeof(char));
                    strncpy(frame->word, first->word, first->length);
                    frame->word[first->length]=c;
                    frame->word[frame->length]=0;
                    frame->node=child;
                    last->next=frame;
                    frame->next=NULL;
                    last=frame;
                }
            }
        }
        frame=(Frame*)first->next;
        free(first->word);
        free(first);
        first=frame;
    }

    return 0;
}

int main(int argc, const char** argv) {

    if( argc<3 ) {
        printf("too few arguments\n");
        exit(1);
    }


    const char* dictFileName=argv[1];
    const char* matrixFileName=argv[2];

    Node* root=dict2tree(dictFileName);
    Matrix* matrix=parseMatrix(matrixFileName);

    int show=0;
    if( show ) {
        printTreeRec(0,root);
    }
    wordsInMatrix(root,matrix);
    if( show ) {
        printTreeRec(0,root);
    }
    int i;
    for( i=3;i<argc;++i ) {
        printf("'%s' %s in tree\n",argv[i],(word_in_tree(root,argv[i])?"is":"is not"));
    }

    

    return 0;
}
