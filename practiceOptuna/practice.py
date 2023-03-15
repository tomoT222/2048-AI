import optuna

def f(x):
    return x**4 - 4 * x ** 3 - 36 * x ** 2

def objective(trial):
    x = trial.suggest_uniform('x', -10, 10)
    return f(x)

study = optuna.create_study()
study.optimize(objective, n_trials=10)

print(study.best_params)

f = open('input.txt', 'r')

data = f.read()
print(data)

f.close()
