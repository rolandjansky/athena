                                                                            
                                                                            
                                                                            
     Data Dependency Graph                                                  
    ========================                                                
                                                                            
       B            A                                                       
       |           / \                                                      
     b1|        a1/   \a2                                                   
       |         /     \                                                    
        \       C       D                                                   
         \     / \      |                                                   
          \ c1/   \c2   |d1                                                 
           \ /     \    |                                                   
            E      |    |                                                   
            \      |    |                                                   
             \e1   |    |                                                   
              \    |   / \                                                  
               \   |  /   \                                                 
                \  | /     G                                                
                 \ |/      |                                                
                   F       |g1                                              
                                                                            
                                                                            
                  \|/                                                       
                   V   : runtime configurable ReadHandleKeyArray            
                                                                            
                                                                            
                                                                            
                                                                            
     Data Dependency Graph for CondHandles
    ========================================                                
                                                                            
                     A -----------------------\                           
            CX2     /|\      CX1          a1   \    CY1                    
            /\     / | \      /\                \    |\                      
           /X2\ a2/  |  \a1  /X1\                \ Y1| \Y2                   
          /    \ /   |a1 \  /    \                \  |  |                    
          |     C2   B    C1     |                 \ | /                       
          \           \_________ |                  \|/                        
           \               a3   \|                   D2                       
            \-------------------D1                                          
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
