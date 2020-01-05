from mpi4py import MPI
import numpy as np
import random
import sys


def main(args):
	comm = MPI.COMM_WORLD
	rank = comm.Get_rank()
	size = comm.Get_size()
	
	epochs = int(args[1])
	steps_per_epoch = int(args[2])
	x = np.array(eval(args[3]))
	coef = lambda x : eval(args[4])
	grads = args[5:]
	
	if rank == 0:
		history = [x]
		dim = x.size
		for epoch in range(epochs):
			data = np.empty(dim, dtype=np.float64)
			xs = []
			for r in range(1, size):
				comm.Recv(data, source=r, tag=13)
				xs.append(data)
			x = random.choice(xs)
			history.append(x)
			if epoch < epochs - 1:
				for r in range(1, size):
					comm.Send(x, dest=r, tag=13)
			else:
				print(str(history).replace('array', 'np.array'))
	else:
		for epoch in range(epochs):
			for step in range(steps_per_epoch):
				x = x - coef(epoch*steps_per_epoch + step)*np.array(eval(grads[rank-1]))
			comm.Send(x, dest=0, tag=13)
			if epoch < epochs - 1:
				comm.Recv(x, source=0, tag=13)
		
	
if __name__ == "__main__":
	main(sys.argv)

