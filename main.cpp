/*
Copyright (c) 2015-2016, Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

//////////////////////////////////////////////////////////////////////////////

#include <getopt.h>
#include <cstdint>
#include <iostream>
#include "type.h"
#include "variable.h"
#include "node.h"
#include "gen_policy.h"
#include "generator.h"
#include "sym_table.h"
#include "new-master.h"

using namespace rl;

int main (int argc, char* argv[]) {

    extern char *optarg;
    extern int optind;
    char *pEnd;
    std::string out_dir = "./";
    int c;
    uint64_t seed = 0;
    static char usage[] = "usage: [-q -d <out_dir> -s <seed>\n";
    bool opt_parse_err = 0;
    bool quiet = false;

    while ((c = getopt(argc, argv, "qhrd:s:")) != -1)
        switch (c) {
        case 'd':
            out_dir = std::string(optarg);
            break;
        case 's':
            seed = strtoull(optarg, &pEnd, 10);
            break;
        case 'q':
            quiet = true;
            break;
        case 'h':
        default:
            opt_parse_err = true;
            break;
        }
    if (optind < argc) {
        for (; optind < argc; optind++)
            std::cerr << "Unrecognized option: " << argv[optind] << std::endl;
        opt_parse_err = true;
    }
    if (argc == 1 && !quiet) {
        std::cerr << "Using default options" << std::endl;
        std::cerr << "For help type " << argv [0] << " -h" << std::endl;
    }
    if (opt_parse_err) {
        std::cerr << usage << std::endl;
        exit(-1);
    }


    rand_val_gen = std::make_shared<RandValGen>(RandValGen (seed));

    Master mas (out_dir);
    mas.generate ();
    mas.emit_func ();
    mas.emit_init ();
    mas.emit_decl ();
    mas.emit_hash ();
    mas.emit_check ();
    mas.emit_main ();
//    mas.emit_reduce_log ();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test utilities
/*
    std::shared_ptr<Type> type;
    type = IntegerType::init (IntegerType::IntegerTypeID::UINT);
    type->dbg_dump();

    std::shared_ptr<StructType> struct_type;
    struct_type = std::static_pointer_cast<StructType> (StructType::init ("struct_1"));
    struct_type->add_member(type, "a");
    struct_type->add_member(IntegerType::init (IntegerType::IntegerTypeID::BOOL), "b");
    struct_type->dbg_dump();

    std::shared_ptr<StructType> struct_type2;
    struct_type2 = std::static_pointer_cast<StructType> (StructType::init ("struct_2"));
    struct_type2->add_member(IntegerType::init (IntegerType::IntegerTypeID::ULLINT), "c");
    struct_type2->add_member(struct_type, "d");
    struct_type2->add_member(IntegerType::init (IntegerType::IntegerTypeID::CHAR), "e");
    struct_type2->dbg_dump();

    Struct struct_var = Struct ("struct_var", struct_type);
    struct_var.dbg_dump();

    Struct struct_var2 = Struct ("struct_var2", struct_type2);
    struct_var2.dbg_dump();

    Variable var = Variable("i", IntegerType::IntegerTypeID::ULINT, Type::Mod::NTHG, false);
    var.set_modifier (Type::Mod::CONST);
    var.set_value (10);
    var.set_min (5);
    var.set_max (50);
    var.dbg_dump ();

    Array arr = Array ("a", IntegerType::IntegerTypeID::UINT, Type::Mod::CONST_VOLAT, true, 20,
                           Array::Ess::STD_VEC);
    arr.set_align(32);
    arr.dbg_dump();

    VarUseExpr var_use;
    var_use.set_variable (std::make_shared<Variable> (var));
    std::cout << "VarUseExpr: " << var_use.emit () << std::endl;

    AssignExpr assign;
    assign.set_to (std::make_shared<VarUseExpr> (var_use));
    assign.set_from  (std::make_shared<VarUseExpr> (var_use));
    std::cout << "AssignExpr: " << assign.emit () << std::endl;

    IndexExpr index;
    index.set_base (std::make_shared<Array> (arr));
    index.set_index (std::make_shared<VarUseExpr> (var_use));
    index.set_is_subscr(true);
    std::cout << "IndexExpr: " << index.emit () << std::endl;
    index.set_is_subscr(false);
    std::cout << "IndexExpr: " << index.emit () << std::endl;

    MemberExpr member_expr;
    member_expr.set_struct(std::make_shared<Struct>(struct_var));
    member_expr.set_identifier(1);
    member_expr.propagate_type();
    member_expr.propagate_value();
    std::cout << "MemberExpr: " << member_expr.emit () << std::endl;

    MemberExpr member_expr2;
    member_expr2.set_struct(std::make_shared<Struct>(struct_var2));
    member_expr2.set_identifier(1);
    member_expr2.propagate_type();
    member_expr2.propagate_value();
    std::cout << "MemberExpr: " << member_expr2.emit () << std::endl;
    std::cout << "MemberExpr value: " << member_expr2.get_int_type_id() << std::endl;

    BinaryExpr bin_add;
    bin_add.set_op (BinaryExpr::Op::Add);
    bin_add.set_lhs (std::make_shared<VarUseExpr> (var_use));
    bin_add.set_rhs (std::make_shared<VarUseExpr> (var_use));
    std::cout << "BinaryExpr: " << bin_add.emit () << std::endl;

    AssignExpr assign2;
    assign2.set_to (std::make_shared<IndexExpr> (index));
    assign2.set_from  (std::make_shared<BinaryExpr> (bin_add));
    std::cout << "AssignExpr: " << assign2.emit () << std::endl;

    UnaryExpr unary;
    unary.set_op (UnaryExpr::Op::BitNot);
    unary.set_arg (std::make_shared<IndexExpr> (index));
    std::cout << "UnaryExpr: " << unary.emit () << std::endl;

    ConstExpr cnst;
    cnst.set_type (IntegerType::IntegerTypeID::ULLINT);
    cnst.set_data (123321);
    std::cout << "ConstExpr: " << cnst.emit () << std::endl;

    DeclStmt decl;
    decl.set_data (std::make_shared<Array> (arr));
    std::cout << "DeclStmt: " << decl.emit () << std::endl;
    decl.set_is_extern (true);
    std::cout << "DeclStmt: " << decl.emit () << std::endl;

    ExprStmt ex_st;
    ex_st.set_expr (std::make_shared<AssignExpr>(assign2));
    std::cout << "ExprStmt: " << ex_st.emit () << std::endl;

    BinaryExpr cond;
    cond.set_op(BinaryExpr::Op::Le);
    cond.set_lhs (std::make_shared<VarUseExpr> (var_use));
    cond.set_rhs (std::make_shared<ConstExpr> (cnst));

    UnaryExpr step;
    step.set_op (UnaryExpr::Op::PreInc);
    step.set_arg (std::make_shared<VarUseExpr> (var_use));

    DeclStmt iter_decl;
    iter_decl.set_data (std::make_shared<Variable> (var));

    CntLoopStmt cnt_loop;
    cnt_loop.set_loop_type(LoopStmt::LoopID::FOR);
    cnt_loop.add_to_body(std::make_shared<ExprStmt> (ex_st));
    cnt_loop.set_cond(std::make_shared<BinaryExpr> (cond));
    cnt_loop.set_iter(std::make_shared<Variable> (var));
    cnt_loop.set_iter_decl(std::make_shared<DeclStmt> (iter_decl));
    cnt_loop.set_step_expr (std::make_shared<UnaryExpr> (step));
    std::cout << "CntLoopStmt: " << cnt_loop.emit () << std::endl;

    TypeCastExpr tc;
    tc.set_type(IntegerType::init(IntegerType::IntegerTypeID::ULLINT));
    tc.set_expr(std::make_shared<UnaryExpr> (unary));
    std::cout << "TypeCastExpr: " << tc.emit () << std::endl;

    ExprListExpr expr_list;
    expr_list.add_to_list(std::make_shared<TypeCastExpr>(tc));
    expr_list.add_to_list(std::make_shared<UnaryExpr>(step));
    std::cout << "ExprListExpr: " << expr_list.emit () << std::endl;

    FuncCallExpr fc;
    fc.set_name("hash");
    fc.set_args(std::make_shared<ExprListExpr> (expr_list));
    std::cout << "FuncCallExpr: " << fc.emit () << std::endl;

    IfStmt if_stmt;
    if_stmt.set_cond(std::make_shared<BinaryExpr> (cond));
    if_stmt.add_if_stmt(std::make_shared<ExprStmt> (ex_st));
    std::cout << "IfStmt: " << if_stmt.emit () << std::endl;
    if_stmt.set_else_exist(true);
    if_stmt.add_else_stmt(std::make_shared<ExprStmt> (ex_st));
    std::cout << "IfStmt: " << if_stmt.emit () << std::endl;

    ConstExpr lhs;
    lhs.set_type (IntegerType::IntegerTypeID::LLINT);
    lhs.set_data (INT_MAX);
    lhs.propagate_type();
    lhs.propagate_value();
    std::cout << "ConstExpr: " << lhs.emit () << std::endl;

    ConstExpr rhs;
    rhs.set_type (IntegerType::IntegerTypeID::LLINT);
    rhs.set_data(2);
    rhs.propagate_type();
    rhs.propagate_value();
    std::cout << "ConstExpr: " << rhs.emit () << std::endl;

    BinaryExpr bin_expr;
    bin_expr.set_op (BinaryExpr::Op::Shr);
    bin_expr.set_lhs (std::make_shared<ConstExpr> (lhs));
    bin_expr.set_rhs (std::make_shared<ConstExpr> (rhs));
    bin_expr.propagate_type();
    Expr::UB tmp = bin_expr.propagate_value();
    std::cout << "UB: " << tmp << std::endl;
    std::cout << "BinaryExpr: " << bin_expr.emit () << std::endl;
    std::cout << "Type: " << bin_expr.get_int_type_id () << std::endl;
    std::cout << "Value: " << (long long int) bin_expr.get_value () << std::endl;


    rand_val_gen = std::make_shared<RandValGen>(RandValGen (0));

    GenPolicy gen_policy;
    std::cout << "GenPolicy:" << std::endl;
    for (auto i = gen_policy.get_allowed_int_types().begin(); i != gen_policy.get_allowed_int_types().end(); ++i)
        std::cout << (*i).get_prob() << " ";
    std::cout << std::endl;
    gen_policy.set_num_of_allowed_int_types (10);
    gen_policy.rand_init_allowed_int_types ();
    for (auto i = gen_policy.get_allowed_int_types().begin(); i != gen_policy.get_allowed_int_types().end(); ++i)
        std::cout << (*i).get_prob() << " ";

    std::cout << "ScalarTypeGen:" << std::endl;
    ScalarTypeGen scalar_type_gen (std::make_shared<GenPolicy>(gen_policy));
    scalar_type_gen.generate();
    scalar_type_gen.get_type()->dbg_dump();

    gen_policy.set_num_of_allowed_int_types(1);
    gen_policy.rand_init_allowed_int_types ();
    ScalarTypeGen scalar_type_gen2 (std::make_shared<GenPolicy>(gen_policy));
    scalar_type_gen2.generate();
    scalar_type_gen2.get_type()->dbg_dump();
    gen_policy.set_num_of_allowed_int_types(4);
    gen_policy.rand_init_allowed_int_types ();

    std::cout << "ModifierGen:" << std::endl;
    ModifierGen mod_gen (std::make_shared<GenPolicy>(gen_policy));
    mod_gen.generate();
    std::cout << mod_gen.get_modifier() << std::endl;

    gen_policy.set_allow_volatile(true);
    ModifierGen mod_gen2 (std::make_shared<GenPolicy>(gen_policy));
    mod_gen2.generate();
    std::cout << mod_gen2.get_modifier() << std::endl;

    gen_policy.set_allow_volatile(false);
    ModifierGen mod_gen3 (std::make_shared<GenPolicy>(gen_policy));
    mod_gen3.generate();
    std::cout << mod_gen3.get_modifier() << std::endl;

    std::cout << "StaticSpecifierGen:" << std::endl;
    StaticSpecifierGen static_gen (std::make_shared<GenPolicy>(gen_policy));
    static_gen.generate();
    std::cout << static_gen.get_specifier () << std::endl;

    gen_policy.set_allow_static_var(true);
    StaticSpecifierGen static_gen2 (std::make_shared<GenPolicy>(gen_policy));
    static_gen2.generate();
    std::cout << static_gen2.get_specifier () << std::endl;

    std::cout << "StructTypeGen:" << std::endl;
    StructTypeGen struct_type_gen (std::make_shared<GenPolicy>(gen_policy));
    struct_type_gen.generate();
    std::cout << struct_type_gen.get_type()->get_definition() << std::endl;

    std::cout << "StructTypeGen:" << std::endl;
    std::vector<std::shared_ptr<StructType>> nested_type;
    nested_type.push_back(struct_type_gen.get_type());
    StructTypeGen struct_type_gen2 (std::make_shared<GenPolicy>(gen_policy), nested_type);
    struct_type_gen2.generate();
    std::cout << struct_type_gen2.get_type()->get_definition() << std::endl;

    std::cout << "ScalarVariableGen:" << std::endl;
    ScalarVariableGen scalar_var_gen (std::make_shared<GenPolicy>(gen_policy));
    scalar_var_gen.generate();
    scalar_var_gen.get_data()->dbg_dump();

    ScalarVariableGen scalar_var_gen2 (std::make_shared<GenPolicy>(gen_policy), IntegerType::IntegerTypeID::UINT, Type::Mod::CONST_VOLAT, false);
    scalar_var_gen2.generate();
    scalar_var_gen2.get_data()->dbg_dump();

    std::cout << "StructVariableGen" << std::endl;
    StructVariableGen struct_var_gen (std::make_shared<GenPolicy>(gen_policy));
    struct_var_gen.generate();
    struct_var_gen.get_data()->dbg_dump();

    std::cout << "StructVariableGen" << std::endl;
    StructVariableGen struct_var_gen2 (std::make_shared<GenPolicy>(gen_policy), std::static_pointer_cast<StructType>(struct_type_gen2.get_type()));
    struct_var_gen2.generate();
    struct_var_gen2.get_data()->dbg_dump();

    std::cout << "ArrayVariableGen" << std::endl;
    ArrayVariableGen arr_var_gen (std::make_shared<GenPolicy>(gen_policy));
    arr_var_gen.generate();
    arr_var_gen.get_data()->dbg_dump();

    ArrayVariableGen arr_var_gen2 (std::make_shared<GenPolicy>(gen_policy), IntegerType::IntegerTypeID::UCHAR, Type::Mod::CONST, true, 1237, Array::Ess::STD_VEC);
    arr_var_gen2.generate();
    arr_var_gen2.get_data()->dbg_dump();
*/
/*
    std::cout << "DeclStmtGen:" << std::endl;
    DeclStmtGen decl_stmt_gen (std::make_shared<GenPolicy>(gen_policy), Data::VarClassID::VAR);
    decl_stmt_gen.generate();
    std::cout << decl_stmt_gen.get_stmt()->emit() << std::endl;

    DeclStmtGen decl_stmt_gen2 (std::make_shared<GenPolicy>(gen_policy), Data::VarClassID::ARR);
    decl_stmt_gen2.generate();
    std::cout << decl_stmt_gen2.get_stmt()->emit() << std::endl;

    std::cout << "SymbolTable:" << std::endl;
    SymbolTable sym_table;
    sym_table.add_variable (std::static_pointer_cast<Variable>(scalar_var_gen.get_data()));
    sym_table.add_variable (std::static_pointer_cast<Variable>(scalar_var_gen2.get_data()));
    std::cout << sym_table.emit_variable_extern_decl() << std::endl;
    std::cout << sym_table.emit_variable_def() << std::endl;
*/
    return 0;
}
