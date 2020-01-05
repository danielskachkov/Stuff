from mpi4py import MPI
import numpy as np
import sys


def main(args):
	comm = MPI.COMM_WORLD
	rank = comm.Get_rank()
	size = comm.Get_size()
	
	epochs = int(args[1])
	H_min = int(args[2])
	H_max = int(args[3])
	x = np.array(eval(args[4]))
	coef = lambda x : eval(args[5])
	grads = args[6:]
	
	if rank == 0:
		history = [x]
		dim = x.size
		for epoch in range(epochs):
			data = np.empty(dim, dtype=np.float64)
			x = np.zeros(dim, dtype=np.float64)
			for r in range(1, size):
				comm.Recv(data, source=r, tag=13)
				x = x + data
			x = x/(size - 1)
			history.append(x)
			if epoch < epochs - 1:
				for r in range(1, size):
					comm.Send(x, dest=r, tag=13)
			else:
				print(str(history).replace('array', 'np.array'))
	else:
		for epoch in range(epochs):
			H = np.random.randint(H_min, H_max + 1)
			for step in range(H):
				x = x - coef(0)*np.array(eval(grads[rank-1]))
			comm.Send(x, dest=0, tag=13)
			if epoch < epochs - 1:
				comm.Recv(x, source=0, tag=13)
		
	
if __name__ == "__main__":
	main(sys.argv)

