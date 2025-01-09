"""
    
"""

import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

def normalize( df ):
    min = df.min()
    max = df.max()

    df = ( df - min ) / ( max - min )

    return df

plt.rcParams[ "font.family" ] = "MS Gothic"
fig, axes = plt.subplots( 4, 1 )

i = pd.read_csv( "./input.csv" )
o = pd.read_csv( "./output.csv" )
o2 = pd.read_csv( "./output2.csv" )
o3 = pd.read_csv( "./output3.csv" )

i.columns = [ 'input' ]
o.columns = [ 'output cutoff=200' ]
o2.columns = [ 'output cutoff=50' ]
o3.columns = [ 'output 2次 cutoff=200' ]

# i = normalize( i )
# o = normalize( o )
# o2 = normalize( o2 )

i.plot( title="60hz + 400hz をハイパス", ax=axes[0] )
o.plot( ax=axes[1], color='orange' )
o2.plot( ax=axes[2], color='green' )
o3.plot( ax=axes[3], color='red' )
plt.show()