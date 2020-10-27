# Genetic library

This project is about making a fast generic library for genetic algorithms in c++.

## Current state

In it's current state, **phenotype** class defines an individual.
Idea is for the library to be as flexible as possible, so we have only 4 methods that 
user should define:
  - initialization: how is your phenotype created? (you usually want to add randomness to this part, but it is up to the user)
  - fitness: how do you calculate fitness score of a given phenotype?
  - crossover: how do you combine two phenotypes to produce a new one?
  - mutate: how does your phenotype mutate?
This 4 functions are the **core** of the phenotype class without which it cannot function.
You have freedom to define other c++ specific functions like copy constructor, destrcutors, move copy constructors , custom constrcutros, etc...
But is not necessary as reasonable defaults exist.

Class **evolution** uses **phenotype** class with core functions that the user has assigned, and is 
pretty much automated completely (tailored to the first example). Only thing that is up to the user is 
termination critearia and function that creates new offsprings given the last population, but the class comes with 
reasonable defaults.

## Future plans

* abstract everything to **evolution** class
* instead of having 4 core functions for phenotype, have some core functions for the **evolution** class
* have a way of storing and loading evolution progress
* make use of multiple CPU cores
* make new example and modify existing one to work with new library

## Example case
![demo](https://github.com/prithvibellamkonda/aec-genetic-lib/blob/main/out.gif)  
Consider a simple problem: guessing the sentence. Now lets see one way of solving it with our library.  
First, how are we going to represent our problem? With string of course!  
Greay, we know what all genotye will be, now lets implement 4 core functions:
  * initialization:
    - we are simply generating random sequence of characters!
  * fitness:
    - we can cheat here a little bit since we already know the mysteri sentence, so 
    - our fitness function will return number of matched characters (squared, so that we favor good guesses more).
  * crossover:
    - randomly pick each letter from two sentences
  * mutation:
    - randomly change one letter in a sentence
And we are basically done! This is of course possible only because default behaviour of 
**evolution** class, so lets see whats in there.
We mentioned that in our evolution class we can define how the new population is being created, but that defaults are 
already implemented, so lets examine the defaults:
  * default offsrping function:
    - for number of phenotypes do:
      * use predefined roullette selection to pick two phenotypes
      * generete new phenotype using crossover function
      * mutate newly created phenotype
Main method in our evolution class is **evolve**, here is what it does:
  * until termination criterions are met, do:
      - generate new population
      - compare best phenotype from the last and current populations
      - if the new one is better, update the *winner* of evolution
      - otherwise increment no_progress
      - (here we can cheat again and check if the winner in current epoch is the sentence we're looking for)

## How to run
 - Make sure you have c++ compiler with c++ standard 20 support (gcc 10.2.0 is used)
 - run `make`
 - run the executable with `./main`
You should get something simillar to the given demo.




