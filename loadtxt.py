import re

def findall(textline, data_format):
    x = re.findall(data_format,textline)
    X = []
    for k in range(len(x)):
        X.append(float(x[k].replace(',','.')))
    return X


def loadtxt(filename, data_format = r"-?\d+(?:\.\d*)?(?:\,\d*)?(?:[Ee][-+]?\d+)?", skiprows = 0):
    
    datafile = open(filename,'r')
    data = datafile.readlines()
    datafile.close()
    
    n = len(findall(data[skiprows], data_format))
    
    out = [[] for i in range(n)]
    
    for i in range(skiprows, len(data)):
        
        X = findall(data[i], data_format)
        
        for j in range(n):
            out[j].append(X[j])

    for j in range(n):
        out[j] = np.array(out[j])
        
    return out
