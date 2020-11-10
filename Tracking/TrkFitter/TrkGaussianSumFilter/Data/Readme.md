#Format of the  files

The first line of the file looks ``Components,Order,transform code`` e.g ``6   5  1``
- This means that the Material effects are described by 6 Gaussian components mixture
- Each Gaussian components need a weight,mean,variance
- Each of the weight,mean,variance are described by the output of a 5th order polynomial meaning 6th coefficients 
- Transfrorm can be 0 or 1 depending on if we needs to tranform the output of the polynomial in order to get
the weight,mean,variance
 
