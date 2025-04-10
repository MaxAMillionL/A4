/*--------------------------------------------------------------------*/
/* checkerDT.c                                                        */
/* Author:                                                            */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "checkerDT.h"
#include "dynarray.h"
#include "path.h"



/* see checkerDT.h for specification */
boolean CheckerDT_Node_isValid(Node_T oNNode) {
   Node_T oNParent;
   Path_T oPNPath;
   Path_T oPPPath;

   /* Sample check: a NULL pointer is not a valid node */
   if(oNNode == NULL) {
      fprintf(stderr, "A node is a NULL pointer\n");
      return FALSE;
   }

   /* Sample check: parent's path must be the longest possible
      proper prefix of the node's path */
   oNParent = Node_getParent(oNNode);
   if(oNParent != NULL) {
      oPNPath = Node_getPath(oNNode);
      oPPPath = Node_getPath(oNParent);

      if(Path_getSharedPrefixDepth(oPNPath, oPPPath) !=
         Path_getDepth(oPNPath) - 1) {
         fprintf(stderr, "P-C nodes don't have P-C paths: (%s) (%s)\n",
                 Path_getPathname(oPPPath), Path_getPathname(oPNPath));
         return FALSE;
      }
   }

   return TRUE;
}

/*
   Performs a pre-order traversal of the tree rooted at oNNode.
   Returns FALSE if a broken invariant is found and
   returns TRUE otherwise. Keeps track of how many nodes are touched 
   through pointer *recurCount.

   You may want to change this function's return type or
   parameter list to facilitate constructing your checks.
   If you do, you should update this function comment.
*/
static boolean CheckerDT_treeCheck(Node_T oNNode, size_t *recurCount) {
   size_t ulIndex;
   Node_T prevNode;
   Path_T currNodePath;
   Path_T prevNodePath;
   prevNode = NULL;

   assert(recurCount != NULL);

   if(oNNode!= NULL) {

      /* Sample check on each node: node must be valid */
      /* If not, pass that failure back up immediately */
      if(!CheckerDT_Node_isValid(oNNode))
         return FALSE;

      /* Recur on every child of oNNode */
      for(ulIndex = 0; ulIndex < Node_getNumChildren(oNNode); ulIndex++)
      {
         Node_T oNChild = NULL;
         int iStatus = Node_getChild(oNNode, ulIndex, &oNChild);

         if(iStatus != SUCCESS) {
            fprintf(stderr, "getNumChildren claims more children than getChild returns\n");
            return FALSE;
         }

        
        if(ulIndex != 0){
            prevNodePath = Node_getPath(prevNode);
            currNodePath = Node_getPath(oNChild);
            
            if(Path_comparePath(prevNodePath, currNodePath) > 0){
                fprintf(stderr, "Two paths are not in lexicographic order in directory\n");
                return FALSE;
            }
            if(Path_comparePath(prevNodePath, currNodePath) == 0){
                fprintf(stderr, "Two children with same name in directory\n");
                return FALSE;
            }
        }
        prevNode = oNChild;
        
        

         /* if recurring down one subtree results in a failed check
            farther down, passes the failure back up immediately */
         if(!CheckerDT_treeCheck(oNChild, recurCount))
            return FALSE;
      }
      (*recurCount)++;
   }
   
   return TRUE;

}

/* see checkerDT.h for specification */
boolean CheckerDT_isValid(boolean bIsInitialized, Node_T oNRoot,
                          size_t ulCount) {
   
   
   size_t recurCount;
   size_t *precurCount;
   int iSuccess;
   recurCount=0;
   precurCount= &recurCount;

   /* Sample check on a top-level data structure invariant:
      if the DT is not initialized, its count should be 0. */
   if(!bIsInitialized){
      if(ulCount != 0) {
         fprintf(stderr, "Not initialized, but count is not 0\n");
         return FALSE;
      }
      if(oNRoot != NULL){
        fprintf(stderr, "Not initialized, but root is not null\n");
        return FALSE;
      }
    }
    if(bIsInitialized){
        if(oNRoot == NULL && ulCount > 0){
            fprintf(stderr, "Root is null, but size is not 0\n");
            return FALSE;
        }
        if(oNRoot != NULL && ulCount == 0){
            fprintf(stderr, "Tree has nodes, but size is 0\n");
            return FALSE;
        }
    }
   /* Now checks invariants recursively at each node from the root. */

   iSuccess = CheckerDT_treeCheck(oNRoot, precurCount);
   if (iSuccess && *precurCount != ulCount)
   {
      fprintf(stderr, "The amount of nodes traversed through does not reflect the number of nodes present currently\n");
      return FALSE;
   } 
   return iSuccess; 

}
