# Chapter 3: Python MPI examples

All Chapter 3 application code uses Python and `mpi4py`. NumPy is used for typed buffers. The older `.c` files are retained as legacy material and are not used by the chapter.

## Installation

The chapter provides full Ubuntu/Debian, macOS Homebrew, Windows WSL 2, and native Windows MS-MPI guides. Windows students following the Open MPI route should use Ubuntu under WSL 2 and execute the Linux commands there.

For a self-contained local environment on supported Linux/macOS machines, run these commands from this directory:

```sh
python3 -m venv .venv-mpi
source .venv-mpi/bin/activate
python -m pip install --upgrade pip
python -m pip install mpi4py openmpi numpy
which python
which mpiexec
python -c "from mpi4py import MPI; print(MPI.Get_library_version())"
```

This wheel-based runtime is an alternative to system Open MPI. Choose one installation route and keep its launcher, library, and Python interpreter consistent. On clusters, use the site's supported MPI and allocation instructions.

## Check syntax and run

Python application scripts need no `mpicc` compilation. `py_compile` produces bytecode and checks syntax; it does not execute MPI communication.

```sh
python -m py_compile hello.py messages.py ping_pong.py token_ring.py parallel_sum.py
mpiexec -n 4 python -m mpi4py hello.py
mpiexec -n 2 python -m mpi4py messages.py
mpiexec -n 2 python -m mpi4py ping_pong.py
mpiexec -n 4 python -m mpi4py token_ring.py
mpiexec -n 4 python -m mpi4py parallel_sum.py 17
```

You can also launch directly with `mpiexec -n 4 python hello.py` or, with Open MPI, `mpirun -np 4 python hello.py`. The `-m mpi4py` runner aborts the job on an unhandled exception, helping prevent peers from remaining blocked after another rank fails.

For native Windows, install matching x64 Python and MS-MPI, create a venv in this directory, and use PowerShell:

```powershell
py -3 -m venv .venv
.\.venv\Scripts\python.exe -m pip install mpi4py numpy
mpiexec -n 4 .\.venv\Scripts\python.exe -m mpi4py hello.py
```

Use that explicit interpreter path for the other scripts too. Never mix a native Windows interpreter with a WSL launcher.

## Expected results

| Program | Ranks | Check |
|---|---:|---|
| `hello.py` | 1, 2, 4 | One rank line per process; order varies. |
| `messages.py` | 2 | Source 0, tag 10, count 3, values `[1.5, 2.5, 3.5]`; dictionary containing `MPI`. |
| `ping_pong.py` | 2 | Mean round-trip time in microseconds; depends on hardware and placement. |
| `token_ring.py` | 1, 2, 4 | Final token 0, 1, 6 respectively. |
| `parallel_sum.py 17` | 1, 2, 4 | Sum 153 and `PASS`. |
| `parallel_sum.py 16` | 4 | Sum 136 and `PASS`. |
| `parallel_sum.py 3` | 4 | Sum 6 and `PASS`; one empty partition. |
| `parallel_sum.py 0` | 4 | Sum 0 and `PASS`; all partitions empty. |

The sum accepts N in 0..1000000 and uses Python integer arithmetic. Invalid N and incorrect process counts for the two-rank programs should terminate with explanatory messages. Empty lists are valid object messages; their serialized payload is not a zero-count buffer message.

For small local correctness tests, Open MPI supports `--oversubscribe` if the launcher reports too few slots. Avoid oversubscribed timing comparisons.

## References

- [mpi4py installation](https://mpi4py.readthedocs.io/en/stable/install.html)
- [mpi4py tutorial](https://mpi4py.readthedocs.io/en/stable/tutorial.html)
- [Windows WSL installation](https://learn.microsoft.com/en-us/windows/wsl/install)
- [Microsoft MPI](https://learn.microsoft.com/en-us/message-passing-interface/microsoft-mpi)
