import optuna

def f(x):
    return x**4 - 4 * x ** 3 - 36 * x ** 2

def objective(trial):
    x = trial.suggest_uniform('x', -10, 10)
    return f(x)

study = optuna.create_study()
study.optimize(objective, n_trials=10)

print(study.best_params)

f1 = open('practiceOptuna/input.txt', 'r')
data = f1.read()
f1.close()

print(data)

f2 = open('practiceOptuna/output.txt', 'w')
f2.write(data)
f2.close()
